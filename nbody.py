

from py.api import *
from py.frontend.PYGAMEwindow import PYGAMEwindow as Window

import random
import time

DRAW = False
SCREEN_W = 800
SCREEN_H = 600

class Nbody():

    class Point():
        @classmethod
        def random_unit(cls):
            return cls(random.random(), random.random(), random.random())

        def __init__(self, x=0, y=0, z=0):
            self.x = 0
            self.y = 0
            self.z = 0

        def __str__(self):
            return f'({self.x}, {self.y}, {self.y})'


    class Body():
        def __init__(self, mass, pos, vel, acc):
            self.mass = mass
            self.pos = pos
            self.vel = vel
            self.acc = acc

        def __str__(self):
            return f'mass({self.mass}), pos{self.pos}, vel{self.vel}, acc{self.acc}'


    def __init__(self, nr_bodies=10):
        self.bodies = []
        self.window = None
        self.prev_pos = [(0, 0)] * nr_bodies
        for i in range(nr_bodies):
            mass = random.random() * 1e10
            pos = self.Point.random_unit()
            vel = self.Point.random_unit()
            acc = self.Point()
            self.bodies.append(self.Body(mass, pos, vel, acc))

    def update_acc(self):
        pass

    def update_vel(self):
        pass

    def update_pos(self):
        pass

    def draw(self):
        if not DRAW:
            return
        if self.window is None:
            self.window = Window(SCREEN_W, SCREEN_H)
            gui_create(self.window)

        max_mass = 0
        for b in self.bodies:
            max_mass = max(max_mass, b.mass)
        for b in self.bodies:
            gui_draw_circle(self.window, b.pos.x, b.pos.y, r, COLOR_GREEN)
            r = b.mass / max_mass * (SCREEN_W / 200)
            gui_draw_circle(self.window, b.pos.x, b.pos.y, r, COLOR_GREEN)


    def run(self, steps):
        while steps > 0:
            steps -= 1
            self.update_acc()
            self.update_vel()
            self.update_pos()
            self.draw()

def main():
    Nbody(10).run(100)

if __name__ == '__main__':
    main()
