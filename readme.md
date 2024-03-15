# IRC Server (ircserv)

## Overview

This project implements an IRC (Internet Relay Chat) server in C++ 98. The server is capable of handling multiple clients simultaneously without hanging and utilizes non-blocking I/O operations. The server allows clients to connect, authenticate, set nicknames, join channels, send and receive private messages, and interact with channels. It supports basic IRC functionalities along with features specific to channel operators such as kicking users, inviting users, changing/viewing channel topics, and modifying channel modes.

## Usage

To run the IRC server, follow these steps:

1. Clone the repository:
    ```
    git clone https://github.com/zakarm/internet-relay-chat.git
    ```

2. Navigate to the project directory:
    ```
    cd ircserv
    ```

3. Build the server:
    ```
    make
    ```

4. Start the server:
    ```
    ./ircserv <port> <password>
    ```

## Features

- Multiple clients can connect to the server simultaneously.
- Non-blocking I/O operations ensure the server doesn't hang.
- Clients can connect, authenticate, and set nicknames.
- Clients can join channels, send and receive private messages.
- Channel operators have additional functionalities like kicking users, inviting users, changing/viewing channel topics, and modifying channel modes.

## Dependencies

- C++ 98
- Make

## Contributing

Contributions are welcome! If you find any bugs or have suggestions for improvements, please open an issue or submit a pull request.


## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.