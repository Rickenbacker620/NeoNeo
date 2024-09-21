import zmq

context = zmq.Context()
socket = context.socket(zmq.SUB)  # Subscriber socket
socket.connect("tcp://localhost:12345")

# Subscribe to all topics
socket.setsockopt_string(zmq.SUBSCRIBE, "")

print("PUB Client is waiting for messages...")

while True:
    message = socket.recv_string()
    print(f"Received: {message}")
