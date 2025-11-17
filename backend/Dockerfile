# Multi-stage build for C++ TrainTrack Backend
# Build stage
FROM gcc:13 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    git \
    libssl-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY CMakeLists.txt ./
COPY include ./include
COPY src ./src
COPY lib ./lib

# Download and install cpp-httplib
RUN git clone --depth 1 https://github.com/yhirose/cpp-httplib.git /tmp/cpp-httplib && \
    cd /tmp/cpp-httplib && \
    mkdir -p /app/lib/cpp-httplib && \
    cp httplib.h /app/lib/cpp-httplib/

# Download and install nlohmann/json
RUN git clone --depth 1 --branch v3.11.3 https://github.com/nlohmann/json.git /tmp/json && \
    mkdir -p /app/lib/nlohmann && \
    cp /tmp/json/single_include/nlohmann/json.hpp /app/lib/nlohmann/

# Build the application
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -j$(nproc)

# Runtime stage
FROM ubuntu:24.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libssl3 \
    ca-certificates \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -u 1001 traintrack

# Set working directory
WORKDIR /app

# Copy binary from builder
COPY --from=builder /app/build/traintrack_server /app/

# Copy configuration
COPY config.json /app/

# Create data directory
RUN mkdir -p /app/data && chown -R traintrack:traintrack /app

# Switch to non-root user
USER traintrack

# Expose port
EXPOSE 18080

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:18080/api/search || exit 1

# Run the server
CMD ["./traintrack_server"]
