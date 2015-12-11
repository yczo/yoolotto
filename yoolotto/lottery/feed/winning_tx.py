import os
import datetime
import urllib
from xml.etree import ElementTree
from django.conf import settings
from yoolotto.lottery.game.manager import GameManager
from yoolotto.lottery.models import LotteryCountryDivision
from yoolotto.lottery.models import LotteryGame
from yoolotto.lottery.models import LotteryGameComponent
from yoolotto.lottery.models import LotteryDraw
from yoolotto.lottery.tasks import notify_draw_result_all
from yoolotto.lottery.tasks import notify_frenzy_all
import pprint


class LotteryWinningTXImporter(object):

	def __init__(self, provider):
		self._provider = provider

	def run(self):
        # Debug / Logging of Imports
		if not os.path.exists(settings.IMPORT_LOG_PATH):
		    os.mkdir(settings.IMPORT_LOG_PATH)

		path = os.path.join(settings.IMPORT_LOG_PATH, datetime.datetime.now().strftime("%Y-%m-%dT%H-%M-%S"))
		if not os.path.exists(path):
		    os.mkdir(path)
		    
		file_dump = os.path.join(path, "import_tx_winning.xml")
		file_log = os.path.join(path, "import_tx_winning.log")
		        
		data = self._provider()

		fp_dump = open(file_dump, "w")
		fp_log = open(file_log, "w")

		fp_dump.write(data)
		fp_dump.close()
		earning={}
		earningList=[]
		rules={}
		xml = ElementTree.fromstring(data)
		
		for game in xml.findall('game'):
			if game.get('area')=='US' and game.get('game_name') == "MEGA Millions":
				game_id=game.get('game_id')
				game_name=game.get('game_name')
				area=game.get('area')
				draw_date=game.get('draw_date')
				updated=game.get('updated')
				component = \
				LotteryGameComponent.objects.get(name=game_name,
												remote_id=game_id,
												division__id=1
												)
				try:
					draw = \
					LotteryDraw.objects.get(date = datetime.datetime.strptime(draw_date, '%Y-%m-%d').date(),
											component__format=component.format,
											component__name=component.name,
											division_id=1
											)
					
					for child in game:
						match=child.find('match').text
						if child.find('prize') is not None:
							prize=child.find('prize').text
						else:
							prize=0
						num_winners=child.find('num_winners').text
						# For MegaMillion
						if match == "4 of 5 + Megaball":
							draw.four_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '3 of 5 + Megaball':
							draw.three_of_five_with_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '2 of 5 + Megaball':
							draw.two_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '1 of 5 + Megaball':
							draw.one_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == 'Megaball Only':
							draw.megaball = int(float(prize.replace('$','').replace(',','')))
						
						elif match == "4 of 5 + Mega Ball":
							draw.four_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '3 of 5 + Mega Ball':
							draw.three_of_five_with_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '2 of 5 + Mega Ball':
							draw.two_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '1 of 5 + Mega Ball':
							draw.one_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == 'Mega Ball Only':
							draw.megaball = int(float(prize.replace('$','').replace(',','')))
						draw.save()
						print "TX Updated Draw ID %s" % draw.id
				except Exception as e:
					print e

			if(game.get('area')=='TX'):
				game_id=game.get('game_id')
				game_name=game.get('game_name')
				area=game.get('area')
				draw_date=game.get('draw_date')
				updated=game.get('updated')
				component = \
				LotteryGameComponent.objects.get(name=game_name,
												remote_id=game_id,
												division__id=1
												)
				try:
					# Now get the draw object
					draw = \
					LotteryDraw.objects.get(date = datetime.datetime.strptime(draw_date, '%Y-%m-%d').date(),
											component__format=component.format,
											component__name=component.name,
											division_id=1
											)
					
					for child in game:
						match=child.find('match').text
						if child.find('prize') is not None:
							prize=child.find('prize').text
						else:
							prize=0
						num_winners=child.find('num_winners').text
						# Powerball
						if match == '5 of 5':
							draw.five_of_five_only = int(float(prize.replace('$','').replace(',','')))
						elif match == '4 of 5 + Powerball':
							draw.four_of_five_powerball = int(float(prize.replace('$','').replace(',','')))
						elif match == '4 of 5':
							draw.four_of_five_only = int(float(prize.replace('$','').replace(',','')))
						elif match == '3 of 5 + Powerball':
							draw.three_of_five_with_powerball = int(float(prize.replace('$','').replace(',','')))
						elif match == '3 of 5':
							draw.three_of_five_only = int(float(prize.replace('$','').replace(',','')))
						elif match == '2 of 5 + Powerball':
							draw.two_of_five_powerball = int(float(prize.replace('$','').replace(',','')))
						elif match == '1 of 5 + Powerball':
							draw.one_of_five_powerball = int(float(prize.replace('$','').replace(',','')))
						elif match == 'Powerball Only':
							draw.powerball_only = int(float(prize.replace('$','').replace(',','')))

						# For MegaMillion
						elif match == "4 of 5 + Megaball":
							draw.four_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '3 of 5 + Megaball':
							draw.three_of_five_with_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '2 of 5 + Megaball':
							draw.two_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '1 of 5 + Megaball':
							draw.one_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == 'Megaball Only':
							draw.megaball = int(float(prize.replace('$','').replace(',','')))
						
						elif match == "4 of 5 + Mega Ball":
							draw.four_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '3 of 5 + Mega Ball':
							draw.three_of_five_with_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '2 of 5 + Mega Ball':
							draw.two_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == '1 of 5 + Mega Ball':
							draw.one_of_five_megaball = int(float(prize.replace('$','').replace(',','')))
						elif match == 'Mega Ball Only':
							draw.megaball = int(float(prize.replace('$','').replace(',','')))
						#Lotto 
						elif match == '6 of 6' and game_name == "Lotto":
							draw.six_of_six_only = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '5 of 6':
							draw.five_of_six_only = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '4 of 6':
							draw.four_of_six_only = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '3 of 6':
							draw.three_of_six_only = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '2 of 6':
							draw.two_of_six_only = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '5 of 6 w/Extra':
							draw.five_of_six_extra = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '4 of 6 w/Extra':
							draw.four_of_six_extra = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '3 of 6 w/Extra':
							draw.three_of_six_extra = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '2 of 6 w/Extra':
							draw.two_of_six_extra = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						#Two Step
						elif match == '4 of 4 w/Bonus':
							draw.four_of_four_bonus = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '4 of 4':
							draw.four_of_four = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '3 of 4 w/Bonus':
							draw.three_of_four_bonus = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '3 of 4':
                                                        draw.three_of_four= int(float(prize.replace('$','').replace(',',''))) if prize else 0
                                                elif match == '2 of 4 w/Bonus':
							draw.two_of_four_bonus = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '1 of 4 w/Bonus':
							draw.one_of_four_bonus = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '0 of 4 w/Bonus':
							draw.bonus = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						# All or Nothing
						elif match == '12 of 12':
							draw.twelve_of_twelve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '11 of 12':
							draw.eleven_of_twelve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '10 of 12':
							draw.ten_of_tweleve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '9 of 12':
							draw.nine_of_twelve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '8 of 12':
							draw.eight_of_twelve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '4 of 12':
							draw.four_of_twelve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '3 of 12':
							draw.three_of_twelve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '2 of 12':
							draw.two_of_twelve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '1 of 12':
							draw.one_of_twelve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
						elif match == '0 of 12':
							draw.zero_of_twelve = int(float(prize.replace('$','').replace(',',''))) if prize else 0
											
						draw.save()
						print "TX Updated Draw ID %s" % draw.id
				except Exception as e:
					print e
					pass
