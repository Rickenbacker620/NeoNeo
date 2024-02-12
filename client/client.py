import socket
import time

SERVER_IP = "127.0.0.1"
SERVER_PORT = 12345
MAX_RETRIES = 5


class Text:
    def __init__(self, byte_string):
        tokens = byte_string.split("\x02")
        self.hook_name = tokens[0]
        self.process_id = tokens[1]
        self.hook_address = tokens[2]
        self.text_context = tokens[3]
        self.text_context2 = tokens[4]
        self.text_content = [tokens[5]]

        self.id = f"{self.hook_name} at process {self.process_id} with context #{self.text_context}"
        if self.text_context2:
            self.id += f"|{self.text_context2}"

    def __hash__(self) -> int:
        return hash(f"{self.process_id}{self.hook_address}{self.text_context}{self.text_context2}")

    def __str__(self):
        return f"{self.id}\n{self.text_content}\n"

    def __repr__(self):
        return f"{self.hook_name} {self.process_id} {self.hook_address} {self.text_context} {self.text_context2} {self.text_content}"

    all_texts = []

    @staticmethod
    def add_text(byte_string):
        tokens = byte_string.split("\x02")
        process_id = tokens[1]
        hook_address = tokens[2]
        text_context = tokens[3]
        text_context2 = tokens[4]
        hash_code = hash(f"{process_id}{hook_address}{text_context}{text_context2}")
        exist_texts = [text for text in Text.all_texts if hash(text) == hash_code]

        # If the text already exists, update the content
        if exist_texts:
            exist_texts[0].text_content.append(tokens[5])
            return exist_texts[0]
        else:
            Text.all_texts.append(Text(byte_string))
            return Text.all_texts[-1]



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
                text_buffer = message.decode("shift-jis")
                text_threads = text_buffer.split("\x03")[:-1]
                for text_thread in text_threads:
                    text = Text.add_text(text_thread)
                    print(text)
            except:
                print(message.hex())

        except socket.error as e:
            print(f"Error receiving message: {e}")
            break

    client_socket.close()


if __name__ == "__main__":
    main()
