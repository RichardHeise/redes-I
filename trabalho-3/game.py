#!/usr/bin/env python3

import random

PAIR = 1
TOAK = 2        # three of a kind
TPAIRS = 3      # two pairs
FULL_HOUSE = 4  
SMALL_STR = 5
LARGE_STR = 6
FOAK = 8        # four of a kind
YAHTZEE = 10    # "five of a kind"

def dicesResult(sorted_dices):
    print("The result is: ")
    if (2 in sorted_dices) and (3 in sorted_dices) and (4 in sorted_dices) and (5 in sorted_dices):
        if (1 in sorted_dices):
            print("+SMALL STRAIGHT+")
            return SMALL_STR
        elif (6 in sorted_dices):
            print("+LARGE STRAIGHT+")
            return LARGE_STR
    else:
        for i in range(1,7):
            if (sorted_dices.count(i) == 5):
                print("+YAHTZEE!+")
                return YAHTZEE
        for i in range(1,7):
            if (sorted_dices.count(i) == 4):
                print("+FOUR OF A KIND+")
                return FOAK
        for i in range(1,7):
            if (sorted_dices.count(i) == 3):
                for j in range(1,7):
                    if (i != j) and (sorted_dices.count(j) == 2):
                        print("+FULL-HOUSE+")
                        return FULL_HOUSE
                print("+THREE OF A KIND+")
                return TOAK
        for i in range(1,7):
            if (sorted_dices.count(i) == 2):
                for j in range(1,7):
                    if (i != j) and (sorted_dices.count(j) == 2):
                        print("+TWO PAIRS+")
                        return TPAIRS
                print("+PAIR+")
                return PAIR


def points(type):
  if type == 1:
    return int(PAIR)
  if type == 2:
    return int(TOAK)
  if type == 3:
    return int(TPAIRS)
  if type == 4:
    return int(FULL_HOUSE)
  if type == 5:
    return int(SMALL_STR)
  if type == 6:
    return int(LARGE_STR)
  if type == 7:
    return int(FOAK)
  if type == 8:
    return int(YAHTZEE)

def rollDices():
    dice1 = {"roll" : True, "value" : 0}
    dice2 = {"roll" : True, "value" : 0}
    dice3 = {"roll" : True, "value" : 0}
    dice4 = {"roll" : True, "value" : 0}
    dice5 = {"roll" : True, "value" : 0}

    dices = [dice1, dice2, dice3, dice4, dice5]

    reroll_chances = 2
    print("Dices are rolling...")
    for i in range(1,4):
        print(str(i) + "º phase:")

        for j in range(0,5):
            if dices[j]["roll"] == True:
                dices[j]["roll"] = False
                dices[j]["value"] = random.randint(1,6)

            print(str(dices[j]["value"]) + " ", end='')

        if reroll_chances != 0:
            print("\n")
            print("Reroll phase. Choose up to 5 dices to reroll.")
            print("You have " + str(reroll_chances) + " rerolls")
            rerolling = input("Wanna reroll? 0 to no, 1 to yes\n")

            if str(rerolling) == '1':
                dices_to_reroll = input("Use numbers from 1 to 5 separated by spaces\n")

                for k in range(0, 5):
                    if str(k+1) in dices_to_reroll:
                        dices[k]["roll"] = True
                reroll_chances = reroll_chances - 1

            elif str(rerolling) == '0':
                reroll_chances = 0
        else:
            print("\n")

    result_values = [0,0,0,0,0]

    for w in range(0,5):
        result_values[w] = dices[w]["value"]

    result_values.sort()

    return dicesResult(result_values)

def bet_name(bet_code):
    if bet_code == 1:
        return "PAIR"
    if bet_code == 2:
        return "THREE OF A KIND"
    if bet_code == 3:
        return "TWO PAIRS"
    if bet_code == 4:
        return "FULL-HOUSE"
    if bet_code == 5:
        return "SMALL STRAIGHT"
    if bet_code == 6:
        return "LARGE STRAIGHT"
    if bet_code == 7:
        return "FOUR OF A KIND"
    if bet_code == 8:
        return "YAHTZEE"