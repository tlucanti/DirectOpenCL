
import time

import pygame
import numpy as np
import threading

from .GUIwindow import GUIwindow

keycode_dict = {
    1: 0, # Left mouse -> 0
    2: 3, # middle mouse -> 3
    3: 2, # right mouse -> 2
}

class PYGAMEwindow(GUIwindow):
	def __init__(self, width, height, winid=None):
		if winid is None:
			winid = self
		self.__winid = winid
		self.__callback = None

		self.__loop_thread = threading.Thread(target=self.__main_loop, args=[width, height])
		self.__loop_thread.start()

		self.__started = False
		while not self.__started:
			time.sleep(1e-5)

	def width(self):
		return pygame.display.get_surface().get_size()[0]

	def height(self):
		return pygame.display.get_surface().get_size()[1]

	def mouse(self):
		return pygame.mouse.get_pos()

	def key_hook(self, callback):
		self.__callback = callback

	def draw(self, image):
		#assert image.shape == (self.height(), self.width(), 3)

		pygame.surfarray.pixels3d(self.__surface)[:] = image.transpose(1, 0, 2)
		self.__screen.blit(self.__surface, (0, 0))
		pygame.display.flip()

	def __main_loop(self, width, height):
		pygame.init()
		pygame.display.set_caption("pygame")

		self.__surface = pygame.Surface((width, height))

		self.__screen = pygame.display.set_mode((width, height))
		self.__started = True

		print('start thread')
		while True:
			event = pygame.event.wait()
			# print(event)
			if event.type == pygame.QUIT:
				break
			elif event.type == pygame.KEYDOWN:
				if self.__callback:
					key = keycode_dict.get(event.key, event.key)
					self.__callback(self.__winid, key, True)
			elif event.type == pygame.KEYUP:
				if self.__callback:
					key = keycode_dict.get(event.key, event.key)
					self.__callback(self.__winid, key, False)
			elif event.type == pygame.MOUSEBUTTONDOWN:
				if self.__callback:
					key = keycode_dict.get(event.button, event.button)
					self.__callback(self.__winid, key, True)
			elif event.type == pygame.MOUSEBUTTONUP:
				if self.__callback:
					key = keycode_dict.get(event.button, event.button)
					self.__callback(self.__winid, key, False)
