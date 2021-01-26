
from matplotlib import pyplot as plt


time = [0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]

ticket_30 = [0, 28, 47, 68, 89, 113, 135, 159, 184, 208, 235]
plt.plot(time, ticket_30, label='30 Ticket')

ticket_20 = [0, 8, 22, 37, 54, 78, 91, 107, 126, 148, 168]
plt.plot(time, ticket_20, label='20 Ticket')

ticket_10 = [0, 5, 9, 13, 20, 25, 30, 36, 44, 52, 57]
plt.plot(time, ticket_10, label='10 Ticket')

plt.xlabel('Time')
plt.ylabel('ticks')
plt.title('Lottery scheduling')

plt.legend()

plt.tight_layout()

plt.savefig('plot.png')

plt.show()
