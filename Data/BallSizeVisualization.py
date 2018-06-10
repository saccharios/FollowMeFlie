'''
======================
3D surface (color map)
======================

Demonstrates plotting a 3D surface colored with the coolwarm color map.
The surface is made opaque by using antialiased=False.

Also demonstrates using the LinearLocator and custom formatting for the
z axis tick labels.
'''

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import numpy as np




# Make data.
Y_plot = np.matrix('10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130')
X_plot = np.matrix(' 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0; 10 10 10 10 10 10 10 10 10 10 10 10 10;20 20 20 20 20 20 20 20 20 20 20 20 20;40 40 40 40 40 40 40 40 40 40 40 40 40;60 60 60 60 60 60 60 60 60 60 60 60 60;80 80 80 80 80 80 80 80 80 80 80 80 80')
Z_size = np.matrix('161.767  74.74  49.21  36.51  30.38  26.5  22.79  21.15  19.807 17.483 13.95 12.39 12.22;148.92  86.55 57.21  42.122  32.47  28.245  24.85  19.92  18.67 16.85 15.16 13.79 13.36;0  0  60.51  47.03  36.61  29.83  24.63  21.85  19.15 15.11 14.8 12.82 12.26;0  0  47.7  41.11  36.48 29.34 25.85 22.39 19.25 18.06 15.22 13.59 12.3;0 0 0 0 29.44  25.84  23.25  21.03  18.43 16.855 14.31 14.03 11.86;0 0 0 0 0  23.01  20.21 18.53 17.51 15.43 15.11 13.16 12.53')


fig = plt.figure()
ax = fig.gca(projection='3d')
# Plot the surface.
surf = ax.plot_surface(X_plot, Y_plot, Z_size, cmap=cm.coolwarm,
                        linewidth=0, antialiased=False)

ax.set_xlabel('y')
ax.set_ylabel('x')
ax.set_zlabel('size')
# Add a color bar which maps values to colors.
fig.colorbar(surf, shrink=0.5, aspect=5)
ax.set_title('3d plot, size of ball')
plt.show()

fig_2d = plt.figure()
ax = fig_2d.gca()
for i in range(0,12):
    ax.plot(X_plot[:,i],Z_size[:,i],label=str(Y_plot[0,i])+'cm')

ax.legend() 
ax.set_title('2d plot, size of ball given distance')
plt.show()
