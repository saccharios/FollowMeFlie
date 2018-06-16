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


# plt.close("all")


x_data_z0 = np.array([10 , 20  ,30 , 40 , 50 , 60 , 70 , 80 , 90, 100, 110, 120 ,130 , \
10  ,20 , 30 , 40,  50,  60 , 70, 80 , 90, 100, 110, 120, 130,\
30,  40 , 50  ,60 , 70 , 80 , 90, 100 ,110, 120, 130,\
30 , 40  ,50 , 60 , 70 , 80 , 90, 100, 110, 120, 130,\
50 , 60  ,70,  80,  90, 100 ,110, 120, 130,\
60 , 70  ,80 , 90 ,100, 110, 120, 130])
y_data_z0 = np.array([])

size_data_z0 = np.array([161.767 , 74.74,  49.21 , 36.51 , 30.38 , 26.5 , 22.79 , 21.15 , 19.807, 17.483, 13.95, 12.39, 12.22,\
148.92 , 86.55, 57.21 , 42.122 , 32.47 , 28.245,  24.85 , 19.92,  18.67, 16.85 ,15.16 ,13.79 ,13.36  ,\
60.51 , 47.03 , 36.61 , 29.83  ,24.63 , 21.85,  19.15 ,15.11 ,14.8 ,12.82 ,12.26,\
47.7 , 41.11 , 36.48, 29.34, 25.85, 22.39, 19.25, 18.06 ,15.22, 13.59, 12.3 ,\
29.44 , 25.84  ,23.25  ,21.03,  18.43, 16.855, 14.31, 14.03, 11.86,\
23.01 , 20.21 ,18.53 ,17.51 ,15.43 ,15.11 ,13.16, 12.53])

x_pix_z0 = np.array([])
y_pix_z0 = np.array([])

# Ansatz: z(x) = a/x + b
# Reformulate as  b*x+a = z(x)*x
#
z_x = np.multiply(size_data_z0, x_data_z0)
coeffs = np.polyfit(x_data_z0, z_x, 1)
print(coeffs)
best_fit = np.array([0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
for i in range(0,13):
    best_fit[i] = coeffs[1] /x_data_z0[i] + coeffs[0]
    print(x_data_z0[i], best_fit[i])
#---------------------------------------------------------------------------------------------------------------------------------------------------

# Make data.
X_plot_z0 = np.matrix('10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130')
Y_plot_z0 = np.matrix(' 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0; 10 10 10 10 10 10 10 10 10 10 10 10 10;20 20 20 20 20 20 20 20 20 20 20 20 20;40 40 40 40 40 40 40 40 40 40 40 40 40;60 60 60 60 60 60 60 60 60 60 60 60 60;80 80 80 80 80 80 80 80 80 80 80 80 80')
Z_size_z0 = np.matrix('161.767  74.74  49.21  36.51  30.38  26.5  22.79  21.15  19.807 17.483 13.95 12.39 12.22;148.92  86.55 57.21  42.122  32.47  28.245  24.85  19.92  18.67 16.85 15.16 13.79 13.36;0  0  60.51  47.03  36.61  29.83  24.63  21.85  19.15 15.11 14.8 12.82 12.26;0  0  47.7  41.11  36.48 29.34 25.85 22.39 19.25 18.06 15.22 13.59 12.3;0 0 0 0 29.44  25.84  23.25  21.03  18.43 16.855 14.31 14.03 11.86;0 0 0 0 0  23.01  20.21 18.53 17.51 15.43 15.11 13.16 12.53')


fig = plt.figure()
ax = fig.gca(projection='3d')
# Plot the surface.
surf = ax.plot_surface(Y_plot_z0, X_plot_z0, Z_size_z0, cmap=cm.coolwarm,
                        linewidth=0, antialiased=False)

ax.set_xlabel('y')
ax.set_ylabel('x')
ax.set_zlabel('size')
# Add a color bar which maps values to colors.
fig.colorbar(surf, shrink=0.5, aspect=5)
ax.set_title('3d plot, size of ball, z = 3.6')
plt.show()

# 2d plot, plot size on y axis, x axis constant
fig_2d = plt.figure()
ax = fig_2d.gca()
for i in range(0,12):
    ax.plot(Y_plot_z0[:,i],Z_size_z0[:,i],label=str(X_plot_z0[0,i])+'cm')

ax.legend() 
ax.set_title('2d plot, size of ball given distance (x), z = 3.6')
ax.set_xlabel('y')
ax.set_ylabel('size')
plt.show()

# 2d plot, plot size on x axis, y axis constant
fig_2d = plt.figure()
ax = fig_2d.gca()
for i in range(0,6):
    ax.plot(np.transpose(X_plot_z0[i,:]),np.transpose(Z_size_z0[i,:]),label=str(Y_plot_z0[i,0])+'cm')
    
ax.plot(x_data_z0[0:13],best_fit,label='best fit')  
ax.legend() 
ax.set_title('2d plot, size of ball given y, z = 3.6')
ax.set_xlabel('x')
ax.set_ylabel('size')
plt.show()

#---------------------------------------------------------------------------------------------------------------------------------------------------
# z = 11.9
# X_plot_z1 = np.matrix('10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130;10  20  30  40  50  60  70  80  90 100 110 120 130')
# Y_plot_z1 = np.matrix(' 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0; 10 10 10 10 10 10 10 10 10 10 10 10 10;20 20 20 20 20 20 20 20 20 20 20 20 20;40 40 40 40 40 40 40 40 40 40 40 40 40;60 60 60 60 60 60 60 60 60 60 60 60 60;80 80 80 80 80 80 80 80 80 80 80 80 80')
# Z_size_z1 = np.matrix('0  78.84  51.91  39.01  32.07 25.95 22.64  19.31  17.51 16.08 14.6 13.72 12.3;0  83.43 54.97  40.67  33.53  27.74  22.69  19.21  18.49 16.26 15.14 14.13 12.25;0  0  53.35  41.06  33.61  27.82  24.25  20.16  18.92 17.3 15.35 13.61 13.87;0  0  43.05  38.9 33.35 28.19 24.79 22.54 19.65 17.46 16.41 14.76 13.2;0 0 0 0 29.76  27.7  25.34  22.36  20.68 18.81 16.78 15.13 14.6;0 0 0 0 0  23.71  22.24 19.65 19.32 17.74 15.86 14.65 14.32')
# 
# 
# fig = plt.figure()
# ax = fig.gca(projection='3d')
# # Plot the surface.
# surf = ax.plot_surface(Y_plot_z1, X_plot_z1, Z_size_z1, cmap=cm.coolwarm,
#                         linewidth=0, antialiased=False)
# 
# ax.set_xlabel('y')
# ax.set_ylabel('x')
# ax.set_zlabel('size')
# # Add a color bar which maps values to colors.
# fig.colorbar(surf, shrink=0.5, aspect=5)
# ax.set_title('3d plot, size of ball, z = 11.9')
# plt.show()
# 
# # 2d plot, plot size on y axis, x axis constant
# fig_2d = plt.figure()
# ax = fig_2d.gca()
# for i in range(0,12):
#     ax.plot(Y_plot_z1[:,i],Z_size_z1[:,i],label=str(X_plot_z1[0,i])+'cm')
# 
# ax.legend() 
# ax.set_title('2d plot, size of ball given distance (x), z = 11.9')
# ax.set_xlabel('y')
# ax.set_ylabel('size')
# plt.show()
# 
# # 2d plot, plot size on x axis, y axis constant
# fig_2d = plt.figure()
# ax = fig_2d.gca()
# for i in range(0,6):
#     ax.plot(np.transpose(X_plot_z1[i,:]),np.transpose(Z_size_z1[i,:]),label=str(Y_plot_z1[i,0])+'cm')
# ax.legend() 
# ax.set_title('2d plot, size of ball given y, z = 11.9')
# ax.set_xlabel('x')
# ax.set_ylabel('size')
# plt.show()



