from matplotlib import pyplot as plt

plt.style.use('fivethirtyeight')

slices = (1266, 839, 291)
labels = ('30 Ticket', '20 Ticket', '10 Ticket')
colors = ['#31FA45', '#DBB560', '#75FF70']
plt.pie(slices, labels=labels, colors=colors, wedgeprops={'edgecolor': 'black'}, shadow='True', autopct='%1.1f%%')
plt.tight_layout()
plt.title('Lottery scheduling')
plt.show()
