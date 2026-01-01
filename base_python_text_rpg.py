import pickle
import random
import sys
import os
from random import randint
class Character:
  def __init__(self):
    self.name = ""
    self.health = 200
    self.health_max = 200
    self.damage = 0
  #PlayerDamage
  def do_damage(self, enemy):
    self.damage = min(max(randint(0, self.health) - randint(0, enemy.health), 0),enemy.health)
    enemy.health = enemy.health - self.damage
    if self.damage == 0: print ("Mr.%s evades %s-san's attack." % (enemy.name, self.name))
    else:
      print ("%s-san hurts Mr.%s!" % (self.name, enemy.name))
      print (" damage by player: %d "% self.damage)
    return enemy.health <= 0
  #EnemyDamage
  def do_damage_2(enemy, self):
    print ("Mr.%s's health: %d/%d" % (enemy.name, enemy.health, enemy.health_max))
    damage_2 = min(max(randint(0, enemy.health) - randint(0, self.health), 0),self.health)
    self.health = self.health - damage_2
    if damage_2 == 0: print ("%s-san evades Mr.%s attack." % (self.name, enemy.name))
    else: print (")%s-san hurts Mr.%s!" % (enemy.name, self.name))
    return self.health <= 0

class Enemy(Character):
  def __init__(self, player):
    Character.__init__(self)
    if random.randint(0,1):
      self.name = 'High orcs'
      self.health_max = randint(100, player.health)
      self.health = self.health_max
    else:
      if random.randint(0,1):
        self.name = 'Majin'
        self.health_max = randint(150, player.health)
        self.health = self.health_max
      else:
        if random.randint(0, 1):
          self.name = 'Kajin'
          self.health_max = randint(170, player.health)
          self.health = self.health_max
        else:
          self.name = 'Demon Lord'
          self.health_max = randint(200, player.health)
          self.health = self.health_max

class Player(Character):
  def __init__(self):
    Character.__init__(self)
    #player
    self.state = 'normal'
    self.health = 200
    self.health_max = 200
   
  def quit(self):
    print ("%s-san can't find the way back home, and dies of starvation.\nR.I.P." % self.name)
    self.health = 0
  def help(self): print (Commands.keys())
  def status(self):
    print ("%s-san's health: %d/%d" % (self.name, self.health, self.health_max))
  def tired(self):
    print ("%s-san feels tired." % self.name)
    self.health = max(1, self.health - 10)
  def rest(self):
    if self.state != 'normal': print ("%s-san can't rest now!" % self.name);self.enemy_attacks()
    else:
      if randint(0, 1):
        self.enemy = Enemy(self)
        print ("%s-san is rudely awakened by Mr.%s!" % (self.name, self.enemy.name))
        self.state = 'fight'
        self.enemy_attacks()
      else:
        print ("%s-san rests." % self.name)
        if self.health < self.health_max:
          self.health = self.health_max
        else: print ("%s-san slept too much." % self.name); self.health = self.health - 10
  def explore(self):
    if self.state != 'normal':
      print ("%s-san is too busy right now!" % self.name)
      self.enemy_attacks()
    else:
      if randint(0, 1):
        self.enemy = Enemy(self)
        print ("%s-san encounters a %s!" % (self.name, self.enemy.name))
        self.state = 'fight'
      else:
        print ("%s-san explores a twisty passage." % self.name)
  def flee(self):
    if self.state != 'fight': print( "%s-san runs in circles for a while." % self.name); self.tired()
    else:
      if randint(1, self.health + 5) > randint(1, self.enemy.health):
        self.enemy = Enemy(self)
        print ("%s-san flees from Mr.%s." % (self.name, self.enemy.name))
        self.enemy = None
        self.state = 'normal'
      else:
        self.enemy = Enemy(self)
        print ("%s-san couldn't escape from Mr.%s!" % (self.name, self.enemy.name)); self.enemy_attacks()
  def attack(self):
    if self.state != 'fight': print ("%s-san swats the air, without notable results." % self.name); self.tired()
    else:
      if self.do_damage(self.enemy):
        print ("%s-san executes Mr.%s!" % (self.name, self.enemy.name))
        self.enemy = None
        self.state = 'normal'
        if randint(0, self.health) < self.health_max:
          self.health += 50
          self.strength += 1
          if self.strength >= 3:
            if self.strength % 3 == 0 or self.strength % 3 == 1 or self.strength % 3 == 2:
              self.CombatLevel += int(self.strength/3)
          else:
            self.CombatLevel=0
          print(" damage before lvling: %d" % self.damage)
          self.damage += random.randint(20, 30)
          self.health_max += 50
          self.credit += randint(25, 50)
          print ("%s-san feels stronger!" % self.name)
      else: self.enemy_attacks()
  
  def enemy_attacks(self):
    if self.enemy.do_damage_2(self): print ("%s-san was slaughtered by Mr.%s!!!\nR.I.P." %(self.name, self.enemy.name))
  
Commands = {
  'quit': Player.quit,
  'help': Player.help,
  'status': Player.status,
  'rest': Player.rest,
  'explore': Player.explore,
  'flee': Player.flee,
  'attack': Player.attack,
  }

p = Player()
p.name = input("What is your character's name? ")
print ("(type help to get a list of actions)\n")
print ("%s-san enters a dark cave, searching for adventure." % p.name)
while(p.health > 0):
  line = input("> ")
  args = line.split()
  if len(args) > 0:
    commandFound = False
    for c in Commands.keys():
      if args[0] == c[:len(args[0])]:
        Commands[c](p)
        commandFound = True
        break
    if not commandFound:
      print ("%s-san, doesn't understand the suggestion." % p.name)
 
