#define PORT "3000"
#define BACKLOG 10

int main() {  // Get the listener socket
    int listener = get_listener(PORT, BACKLOG);

    printf("Listening in: 127.0.0.1:%s\n", PORT);

    handle_connection(listener);
}
