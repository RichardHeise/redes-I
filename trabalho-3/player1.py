import socket

UDP_IP = "localhost"
UDP_PORT_SEND = "6662"
UDP_PORT_REC = "6661"

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT_REC))

baton = True
chips = 5

print("All set? Let's play.")
print("You have " + str(chips) + " chips")
while(True):
    if baton == True:
        print("Your turn, place a bet")
        print("""
                1-PAIR
                2-THREE OF A KIND
                3-TWO PAIRS
                4-FULL-HOUSE
                5-SMALL STRAIGHT
                6-LARGE STRAIGHT
                7-FOUR OF A KIND
                8-YAHTZEE
                """)
        bet = int(input("What say you? (bet 1-8)"))


