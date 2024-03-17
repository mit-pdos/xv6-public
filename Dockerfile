FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    gdb \
    qemu \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /xv6

COPY . .

CMD ["bash"]
