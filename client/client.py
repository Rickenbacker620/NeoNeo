import socket
import time

SERVER_IP = '192.168.64.1'
SERVER_PORT = 12345
MAX_RETRIES = 5

def establish_connection():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        client_socket.connect((SERVER_IP, SERVER_PORT))
        return client_socket
    except socket.error as e:
        print(f"Connection failed: {e}")
        return None

def main():
    attempts = 0
    while attempts < MAX_RETRIES:
        print(f"Attempt #{attempts + 1} to connect...")
        client_socket = establish_connection()
        if client_socket:
            print("Connected to server")
            break  # Break out of the loop if connection successful

        attempts += 1
        time.sleep(1)  # Wait before retrying

    if attempts == MAX_RETRIES:
        print("Maximum retries reached. Connection unsuccessful.")
        return

    while True:
        try:
            message = client_socket.recv(2048)
            if not message:
                print("Connection closed by server")
                break
            try:
                print(f"Received message from server: {message.decode('shift-jis')}")
            except:
                print(message.hex())
        except socket.error as e:
            print(f"Error receiving message: {e}")
            break

    client_socket.close()

if __name__ == "__main__":
    main()
