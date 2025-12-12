/**
 * @file Udp_forwarder.cpp
 * @brief UDP packet forwarder that duplicates traffic to multiple destinations
 * 
 * This program listens on a specified UDP port and forwards incoming packets
 * to multiple destination addresses. It supports configurable destinations,
 * rate limiting, verbose logging, and graceful shutdown handling.
 * 
 * @author UDP Forwarder Development Team
 * @date 2025
 * @version 1.0
 * 
 */

/***************************************************************************
# Compile
g++ -std=c++11 -o udp_forwarder udp_forwarder.cpp

# Basic usage
./udp_forwarder 9999 192.168.1.100:8888 192.168.1.101:8888

# With options
./udp_forwarder -v -r 500 9999 10.0.0.1:7777 10.0.0.2:7777 10.0.0.3:7777

# Help
./udp_forwarder --help
**************************************************************************/
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <algorithm>
#include <getopt.h>
/**
 * @brief Maximum UDP payload size for IPv4
 * 
 * Theoretical maximum is 65507 bytes for IPv4 UDP datagrams
 * (65535 - 20 IP header - 8 UDP header)
 */
const size_t MAX_UDP_PAYLOAD = 65507;

/**
 * @brief Default rate limit in packets per second per source IP
 * 
 * Used to prevent potential denial-of-service from a single source
 */
const int DEFAULT_RATE_LIMIT = 1000;

/**
 * @brief Configuration structure for the forwarder
 * 
 * Contains all runtime configuration parameters parsed from
 * command line arguments or configuration files
 */
struct ForwarderConfig {
    uint16_t listen_port;                     ///< Port to listen on for incoming packets
    std::vector<struct sockaddr_in> destinations; ///< List of destination addresses
    bool verbose = false;                     ///< Enable verbose logging output
    int rate_limit = DEFAULT_RATE_LIMIT;      ///< Rate limit in packets per second
    std::string config_file;                  ///< Optional configuration file path
};

/**
 * @brief Rate limiting structure for tracking source IP activity
 * 
 * Tracks packet count and timestamp for rate limiting per source IP
 */
struct RateLimitInfo {
    std::chrono::steady_clock::time_point window_start; ///< Start of current rate limit window
    int packet_count = 0;                               ///< Packet count in current window
};

/**
 * @brief Global flag for graceful shutdown
 * 
 * Set to false when SIGINT or SIGTERM is received to break the main loop
 */
static volatile bool keep_running = true;

/**
 * @brief Configuration for the forwarder instance
 */
static ForwarderConfig g_config;

/**
 * @brief Rate limiting map keyed by source IP address
 */
static std::map<uint32_t, RateLimitInfo> g_rate_map;

/**
 * @brief Signal handler for graceful shutdown
 * 
 * Handles SIGINT (Ctrl+C) and SIGTERM signals by setting the
 * keep_running flag to false, allowing the program to exit cleanly
 * 
 * @param sig Signal number (unused)
 */
void signal_handler(int sig) {
    static int call_count = 0;
    call_count++;
    
    if (call_count >= 3) {
        std::cerr << "\nForcing shutdown after multiple interrupts...\n";
        exit(1);
    }
    
    std::cerr << "\nReceived signal " << sig 
              << ". Shutting down gracefully... (Press Ctrl+C again to force)\n";
    keep_running = false;
}

/**
 * @brief Parse an IP:PORT string into sockaddr_in structure
 * 
 * Converts a string in the format "IP:PORT" to a sockaddr_in structure.
 * Supports both IPv4 addresses and hostnames (via DNS resolution).
 * 
 * @param[in] addr_str String in format "IP:PORT" or "hostname:PORT"
 * @param[out] sockaddr Output sockaddr_in structure
 * @return true if parsing succeeded, false otherwise
 * 
 * @note For hostname resolution, this uses gethostbyname() which is
 *       blocking. Consider non-blocking DNS for production use.
 */
bool parse_address(const std::string& addr_str, struct sockaddr_in& sockaddr) {
    size_t colon_pos = addr_str.find(':');
    if (colon_pos == std::string::npos) {
        std::cerr << "Error: Address must be in format IP:PORT or hostname:PORT\n";
        return false;
    }
    
    std::string host = addr_str.substr(0, colon_pos);
    std::string port_str = addr_str.substr(colon_pos + 1);
    
    // Parse port number
    try {
        int port = std::stoi(port_str);
        if (port < 0 || port > 65535) {
            std::cerr << "Error: Port must be between 0 and 65535\n";
            return false;
        }
        sockaddr.sin_port = htons(static_cast<uint16_t>(port));
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid port number: " << port_str << "\n";
        return false;
    }
    
    // Initialize structure
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    
    // Try to parse as IP address first
    if (inet_pton(AF_INET, host.c_str(), &sockaddr.sin_addr) == 1) {
        return true;
    }
    
    // If not an IP, try hostname resolution
    struct hostent* host_entry = gethostbyname(host.c_str());
    if (host_entry == nullptr) {
        std::cerr << "Error: Could not resolve hostname: " << host << "\n";
        return false;
    }
    
    sockaddr.sin_addr = *reinterpret_cast<struct in_addr*>(host_entry->h_addr);
    return true;
}

/**
 * @brief Check if a source IP is exceeding rate limits
 * 
 * Implements a token bucket algorithm for rate limiting.
 * Each source IP is allowed a configurable number of packets per second.
 * 
 * @param[in] src_ip Source IP address in network byte order
 * @return true if packet should be allowed, false if rate limited
 */
bool check_rate_limit(uint32_t src_ip) {
    if (g_config.rate_limit <= 0) {
        return true; // Rate limiting disabled
    }
    
    auto now = std::chrono::steady_clock::now();
    auto& limit_info = g_rate_map[src_ip];
    
    // Check if we're in a new time window (1 second)
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - limit_info.window_start).count();
    
    if (elapsed >= 1) {
        // Reset counter for new window
        limit_info.packet_count = 0;
        limit_info.window_start = now;
    }
    
    // Check if packet count exceeds limit
    if (limit_info.packet_count >= g_config.rate_limit) {
        return false;
    }
    
    limit_info.packet_count++;
    return true;
}

/**
 * @brief Print usage information to stderr
 * 
 * Displays command line options and examples for the program
 * 
 * @param[in] program_name Name of the executable (argv[0])
 */
void print_usage(const char* program_name) {
    std::cerr << "UDP Forwarder v2.0 - Duplicate UDP traffic to multiple destinations\n\n";
    std::cerr << "Usage: " << program_name << " [OPTIONS] <listen_port> <dest1> [dest2 ...]\n\n";
    std::cerr << "Options:\n";
    std::cerr << "  -v, --verbose       Enable verbose output (show forwarded packets)\n";
    std::cerr << "  -r, --rate LIMIT    Set rate limit in packets/sec per source IP (0 to disable)\n";
    std::cerr << "  -h, --help          Display this help message and exit\n";
    std::cerr << "\nArguments:\n";
    std::cerr << "  listen_port          UDP port to listen on (0-65535)\n";
    std::cerr << "  destN                Destination address in format IP:PORT or hostname:PORT\n";
    std::cerr << "\nExamples:\n";
    std::cerr << "  " << program_name << " 9999 192.168.1.100:8888 192.168.1.101:8888\n";
    std::cerr << "  " << program_name << " -v -r 500 9999 10.0.0.1:7777 10.0.0.2:7777 10.0.0.3:7777\n";
    std::cerr << "  " << program_name << " --help\n";
}

/**
 * @brief Parse command line arguments
 * 
 * Parses options and arguments using getopt_long.
 * Validates input and populates the global configuration.
 * 
 * @param[in] argc Argument count
 * @param[in] argv Argument vector
 * @return true if parsing succeeded, false otherwise
 */
bool parse_arguments(int argc, char** argv) {
    static struct option long_options[] = {
        {"verbose", no_argument, nullptr, 'v'},
        {"rate", required_argument, nullptr, 'r'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "vr:h", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'v':
                g_config.verbose = true;
                break;
            case 'r':
                try {
                    g_config.rate_limit = std::stoi(optarg);
                    if (g_config.rate_limit < 0) {
                        std::cerr << "Error: Rate limit cannot be negative\n";
                        return false;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error: Invalid rate limit value: " << optarg << "\n";
                    return false;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                exit(0);
            case '?':
                return false;
        }
    }
    
    // Parse remaining arguments (non-options)
    int remaining_args = argc - optind;
    if (remaining_args < 2) {
        std::cerr << "Error: Insufficient arguments\n\n";
        print_usage(argv[0]);
        return false;
    }
    
    // Parse listen port
    try {
        int port = std::stoi(argv[optind]);
        if (port < 0 || port > 65535) {
            std::cerr << "Error: Listen port must be between 0 and 65535\n";
            return false;
        }
        g_config.listen_port = static_cast<uint16_t>(port);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid listen port: " << argv[optind] << "\n";
        return false;
    }
    
    // Parse destination addresses
    for (int i = optind + 1; i < argc; i++) {
        struct sockaddr_in dest_addr;
        if (!parse_address(argv[i], dest_addr)) {
            std::cerr << "Error: Failed to parse destination: " << argv[i] << "\n";
            return false;
        }
        g_config.destinations.push_back(dest_addr);
    }
    
    return true;
}

/**
 * @brief Initialize UDP socket for listening
 * 
 * Creates, configures, and binds a UDP socket to the specified port.
 * Sets socket options for optimal performance and reuse.
 * 
 * @return Socket file descriptor on success, -1 on failure
 */
int initialize_socket() {
    // Create UDP socket
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Error: Failed to create socket");
        return -1;
    }
    
    // Set socket options
    int optval = 1;
    
    // Allow address reuse (helpful for quick restarts)
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Warning: Failed to set SO_REUSEADDR");
        // Continue anyway, not fatal
    }
    
    // Increase receive buffer size
    int buffer_size = 1024 * 1024; // 1MB
    if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) < 0) {
        perror("Warning: Failed to set receive buffer size");
    }
    
    // Bind to listen address
    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port = htons(g_config.listen_port);
    
    if (bind(sock_fd, reinterpret_cast<struct sockaddr*>(&listen_addr), sizeof(listen_addr)) < 0) {
        perror("Error: Failed to bind socket");
        close(sock_fd);
        return -1;
    }
    
    return sock_fd;
}

/**
 * @brief Setup signal handlers for graceful shutdown
 * 
 * Registers handlers for SIGINT (Ctrl+C) and SIGTERM using sigaction
 * for better portability and control.
 */
void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, nullptr) < 0) {
        perror("Warning: Failed to set SIGINT handler");
    }
    
    if (sigaction(SIGTERM, &sa, nullptr) < 0) {
        perror("Warning: Failed to set SIGTERM handler");
    }
}

/**
 * @brief Convert sockaddr_in to human-readable string
 * 
 * @param[in] addr Socket address structure
 * @return String representation in format "IP:PORT"
 */
std::string addr_to_string(const struct sockaddr_in& addr) {
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str));
    return std::string(ip_str) + ":" + std::to_string(ntohs(addr.sin_port));
}

/**
 * @brief Main packet forwarding loop
 * 
 * Continuously receives packets and forwards them to all configured
 * destinations. Handles rate limiting and verbose logging.
 * 
 * @param[in] sock_fd Socket file descriptor for listening
 */
void run_forwarder(int sock_fd) {
    std::vector<char> buffer(MAX_UDP_PAYLOAD);
    struct sockaddr_in src_addr;
    socklen_t src_addr_len = sizeof(src_addr);
    
    std::cout << "UDP Forwarder started\n";
    std::cout << "Listening on port: " << g_config.listen_port << "\n";
    std::cout << "Destinations (" << g_config.destinations.size() << "):\n";
    for (const auto& dest : g_config.destinations) {
        std::cout << "  - " << addr_to_string(dest) << "\n";
    }
    std::cout << "Rate limit: " << g_config.rate_limit << " packets/sec per source\n";
    std::cout << "Verbose mode: " << (g_config.verbose ? "enabled" : "disabled") << "\n";
    std::cout << "Press Ctrl+C to stop\n\n";
    
    while (keep_running) {
        // Receive packet
        ssize_t received = recvfrom(sock_fd, buffer.data(), buffer.size(), 0,
                                   reinterpret_cast<struct sockaddr*>(&src_addr), &src_addr_len);
        
        if (received < 0) {
            if (errno == EINTR) {
                // Interrupted by signal, check keep_running flag
                continue;
            }
            perror("Error: Failed to receive packet");
            break;
        }
        
        // Apply rate limiting
        if (!check_rate_limit(src_addr.sin_addr.s_addr)) {
            if (g_config.verbose) {
                char src_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &src_addr.sin_addr, src_ip, sizeof(src_ip));
                std::cout << "[RATE LIMITED] From " << src_ip << ":" 
                          << ntohs(src_addr.sin_port) << " (" << received << " bytes)\n";
            }
            continue;
        }
        
        // Forward to all destinations
        bool all_succeeded = true;
        for (const auto& dest : g_config.destinations) {
            ssize_t sent = sendto(sock_fd, buffer.data(), received, 0,
                                 reinterpret_cast<const struct sockaddr*>(&dest), sizeof(dest));
            
            if (sent < 0) {
                perror("Warning: Failed to forward packet");
                all_succeeded = false;
            } else if (sent != received) {
                std::cerr << "Warning: Partial send (" << sent << "/" << received << " bytes)\n";
                all_succeeded = false;
            }
        }
        
        // Log if verbose mode is enabled
        if (g_config.verbose && all_succeeded) {
            char src_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &src_addr.sin_addr, src_ip, sizeof(src_ip));
            std::cout << "Forwarded " << received << " bytes from " << src_ip << ":" 
                      << ntohs(src_addr.sin_port) << " to " << g_config.destinations.size() 
                      << " destinations\n";
        }
    }
}

/**
 * @brief Main entry point for the UDP forwarder
 * 
 * Orchestrates the entire forwarding process:
 * 1. Parse command line arguments
 * 2. Initialize socket
 * 3. Setup signal handlers
 * 4. Run main forwarding loop
 * 5. Cleanup and exit
 * 
 * @param[in] argc Number of command line arguments
 * @param[in] argv Array of command line argument strings
 * @return 0 on success, non-zero on error
 */
int main(int argc, char** argv) {
    std::cout << "========================================\n";
    std::cout << "      UDP Forwarder v2.0\n";
    std::cout << "========================================\n";
    
    // Parse command line arguments
    if (!parse_arguments(argc, argv)) {
        return 1;
    }
    
    // Initialize UDP socket
    int sock_fd = initialize_socket();
    if (sock_fd < 0) {
        return 1;
    }
    
    // Setup signal handlers for graceful shutdown
    setup_signal_handlers();
    
    try {
        // Run the main forwarding loop
        run_forwarder(sock_fd);
    } catch (const std::exception& e) {
        std::cerr << "Error: Unexpected exception: " << e.what() << "\n";
        close(sock_fd);
        return 1;
    }
    
    // Cleanup
    std::cout << "\nShutting down UDP forwarder...\n";
    close(sock_fd);
    
    // Cleanup rate limiting map (optional, but good practice)
    g_rate_map.clear();
    
    std::cout << "Forwarder stopped successfully\n";
    return 0;
}
