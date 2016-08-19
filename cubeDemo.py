

import os
import sys
import math

import serial
import pygame
from operator import itemgetter
from bottle import route, run, get, request

class Point3D:
    def __init__(self, x = 0, y = 0, z = 0):
        self.x, self.y, self.z = float(x), float(y), float(z)
 
    def rotateX(self, angle):
        """ Rotates the point around the X axis by the given angle in degrees. """
        rad = angle * math.pi / 180
        cosa = math.cos(rad)
        sina = math.sin(rad)
        y = self.y * cosa - self.z * sina
        z = self.y * sina + self.z * cosa
        return Point3D(self.x, y, z)
 
    def rotateY(self, angle):
        """ Rotates the point around the Y axis by the given angle in degrees. """
        rad = angle * math.pi / 180
        cosa = math.cos(rad)
        sina = math.sin(rad)
        z = self.z * cosa - self.x * sina
        x = self.z * sina + self.x * cosa
        return Point3D(x, self.y, z)
 
    def rotateZ(self, angle):
        """ Rotates the point around the Z axis by the given angle in degrees. """
        rad = angle * math.pi / 180
        cosa = math.cos(rad)
        sina = math.sin(rad)
        x = self.x * cosa - self.y * sina
        y = self.x * sina + self.y * cosa
        return Point3D(x, y, self.z)
 
    def project(self, win_width, win_height, fov, viewer_distance):
        """ Transforms this 3D point to 2D using a perspective projection. """
        factor = fov / (viewer_distance + self.z)
        x = self.x * factor + win_width / 2
        y = -self.y * factor + win_height / 2
        return Point3D(x, y, self.z)

class RealTimeCube:
    def __init__(self, win_width = 640, win_height = 480):
        pygame.init()

        self.screen = pygame.display.set_mode((win_width, win_height))
        pygame.display.set_caption("JY901-realtime Cube by qtlintw")
        
        self.clock = pygame.time.Clock()

        self.vertices = [
            Point3D(-1,1,-1),
            Point3D(1,1,-1),
            Point3D(1,-1,-1),
            Point3D(-1,-1,-1),
            Point3D(-1,1,1),
            Point3D(1,1,1),
            Point3D(1,-1,1),
            Point3D(-1,-1,1)
        ]

        # Define the vertices that compose each of the 6 faces. These numbers are
        # indices to the vertices list defined above.
        self.faces  = [(0,1,2,3),(1,5,6,2),(5,4,7,6),(4,0,3,7),(0,4,5,1),(3,2,6,7)]

        # Define colors for each face
        self.colors = [(255,0,255),(255,0,0),(0,255,0),(0,0,255),(0,255,255),(255,255,0)]

        self.angle_x = 0
        self.angle_y = 0
        self.angle_z = 0
    def update(self, x, y, z):
        self.screen.fill((0,32,0))
        t = []
        for v in self.vertices:
            # Rotate the point around X axis, then around Y axis, and finally around Z axis.
            r = v.rotateX(x).rotateY(y).rotateZ(z)
            # Transform the point from 3D to 2D
            p = r.project(self.screen.get_width(), self.screen.get_height(), 256, 4)
            # Put the point in the list of transformed vertices
            t.append(p)

        # Calculate the average Z values of each face.
        avg_z = []
        i = 0
        for f in self.faces:
            z = (t[f[0]].z + t[f[1]].z + t[f[2]].z + t[f[3]].z) / 4.0
            avg_z.append([i,z])
            i = i + 1

        # Draw the faces using the Painter's algorithm:
        # Distant faces are drawn before the closer ones.
        for tmp in sorted(avg_z,key=itemgetter(1),reverse=True):
            face_index = tmp[0]
            f = self.faces[face_index]
            pointlist = [(t[f[0]].x, t[f[0]].y), (t[f[1]].x, t[f[1]].y),
                         (t[f[1]].x, t[f[1]].y), (t[f[2]].x, t[f[2]].y),
                         (t[f[2]].x, t[f[2]].y), (t[f[3]].x, t[f[3]].y),
                         (t[f[3]].x, t[f[3]].y), (t[f[0]].x, t[f[0]].y)]
            pygame.draw.polygon(self.screen,self.colors[face_index],pointlist)

        
        pygame.display.flip()
    def run(self):
        #inital serial port
        jy_sensor = serial.Serial(port=sys.argv[1], baudrate="9600", timeout=2)
        print(jy_sensor.name)
        try:
            while True:
                data = jy_sensor.read(size=1)
                if data == b'\x55':
                    jy_sensor.read(size=10)
                    print("success!")
                    break;
                print("trying", data)
            
            #main loop
            while 1:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        pygame.quit()
                        sys.exit()
    
                
                
                data = jy_sensor.read(size=11)
                if not len(data) == 11:
                    print("error byte lenth:", len(data), "data=", data)
                    continue
                if data[0] != b'\x55':
                    print("error occurr, nor 55 start:", data[0])
                    break
                if data[1] == 83:
                    self.angle_x = int.from_bytes(data[2:4], byteorder='little')/32768*180
                    self.angle_y = int.from_bytes(data[4:6], byteorder='little')/32768*180
                    self.angle_z = int.from_bytes(data[6:8], byteorder='little')/32768*180
                    print("Angle output:{}, {}, {}".format(self.angle_x, self.angle_y, self.angle_z))
                else:
                    print(data)
                    continue
                #print("-----", data[0], data[1])
    
        	    # It will hold transformed vertices.
                #self.clock.tick(50)
                self.screen.fill((0,32,0))
                t = []
                for v in self.vertices:
                    # Rotate the point around X axis, then around Y axis, and finally around Z axis.
                    r = v.rotateX(self.angle_x).rotateY(self.angle_y).rotateZ(self.angle_z)
                    # Transform the point from 3D to 2D
                    p = r.project(self.screen.get_width(), self.screen.get_height(), 256, 4)
                    # Put the point in the list of transformed vertices
                    t.append(p)
    
                # Calculate the average Z values of each face.
                avg_z = []
                i = 0
                for f in self.faces:
                    z = (t[f[0]].z + t[f[1]].z + t[f[2]].z + t[f[3]].z) / 4.0
                    avg_z.append([i,z])
                    i = i + 1
    
                # Draw the faces using the Painter's algorithm:
                # Distant faces are drawn before the closer ones.
                for tmp in sorted(avg_z,key=itemgetter(1),reverse=True):
                    face_index = tmp[0]
                    f = self.faces[face_index]
                    pointlist = [(t[f[0]].x, t[f[0]].y), (t[f[1]].x, t[f[1]].y),
                                 (t[f[1]].x, t[f[1]].y), (t[f[2]].x, t[f[2]].y),
                                 (t[f[2]].x, t[f[2]].y), (t[f[3]].x, t[f[3]].y),
                                 (t[f[3]].x, t[f[3]].y), (t[f[0]].x, t[f[0]].y)]
                    pygame.draw.polygon(self.screen,self.colors[face_index],pointlist)
    
                
                pygame.display.flip()
        except KeyboardInterrupt:
            jy_sensor.close()
            print("close port")


c = RealTimeCube()
c.update(0, 0, 0)

@get('/sensor')
def updateSensor():
    orientation_x = request.GET.get("ori_x")
    orientation_y = request.GET.get("ori_y")
    orientation_z = request.GET.get("ori_z")
    c.update(orientation_x, orientation_y, orientation_z)
    return ""
run(host='localhost', port=8000, reloader=True)
