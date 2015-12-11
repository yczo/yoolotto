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


class LotteryWinningImporter(object):

	def __init__(self, provider):
		self._provider = provider

	def run(self):
        # Debug / Logging of Imports
		if not os.path.exists(settings.IMPORT_LOG_PATH):
		    os.mkdir(settings.IMPORT_LOG_PATH)

		path = os.path.join(settings.IMPORT_LOG_PATH, datetime.datetime.now().strftime("%Y-%m-%dT%H-%M-%S"))
		if not os.path.exists(path):
		    os.mkdir(path)
		    
		file_dump = os.path.join(path, "import_ca_winning.xml")
		file_log = os.path.join(path, "import_ca_winning.log")
		        
		data = self._provider()

		fp_dump = open(file_dump, "w")
		fp_log = open(file_log, "w")

		fp_dump.write(data)
		fp_dump.close()
		rules={}
		xml = ElementTree.fromstring(data)
		
		for game in xml.findall('game'):
			if(game.get('area')=='CA'):
				game_id=game.get('game_id')
				game_name=game.get('game_name')
				area=game.get('area')
				draw_date=game.get('draw_date')
				updated=game.get('updated')
				component = \
				LotteryGameComponent.objects.get(name=game_name,
												remote_id=game_id,
												division__id=2
												)
				try:
					
					# Now get the draw object
					draw = \
					LotteryDraw.objects.get(date = datetime.datetime.strptime(draw_date, '%Y-%m-%d').date(),
											component__format=component.format,
											component__name=component.name,
											division_id=2
											)
					
					for child in game:
						match=child.find('match').text
						if child.find('prize') is not None:
							prize=child.find('prize').text
						else:
							prize=0
						num_winners=child.find('num_winners').text
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
						# For Superlotto Plus
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
						# For Fantasy Five Jacpot Mismatch
						elif match == "5 of 5 (Jackpot)" and game_name == "Fantasy 5":
							draw.jackpot = int(float(prize.replace('$','').replace(',','')))
						elif match == 'Straight':
							draw.straight = prize.replace('$','').replace(',','').replace('.00','') if prize else 0
						elif match == 'Box':
							draw.box = prize.replace('$','').replace(',','').replace('.00','') if prize else 0
						elif match == 'Straight and Box':
							draw.staright_and_box = prize.replace('$','').replace(',','').replace('.00','') if prize else 0
						elif match == 'Box Only':
							draw.box_only = prize.replace('$','').replace(',','').replace('.00','') if prize else 0
						
						elif match == 'Exacta and Race Time':
							draw.exacta_with_racetime = prize.replace('$','').replace(',','').replace('.00','') if prize else 0
						elif match == 'Win and Race Time':
							draw.win_with_racetime = prize.replace('$','').replace(',','').replace('.00','') if prize else 0
						elif match == 'Race Time Only':
							draw.race_time_amount = prize.replace('$','').replace(',','').replace('.00','') if prize else 0
						elif match == 'Trifecta':
							draw.trifecta = prize.replace('$','').replace(',','').replace('.00','') if prize else 0
						elif match == 'Exacta':
							draw.exacta = prize.replace('$','').replace(',','').replace('.00','') if prize else 0
						elif match == 'Win':
							draw.win = prize.replace('$','').replace(',','').replace('.00','') if prize else 0

						draw.save()
						print "CA Updated Draw ID %s" % draw.id
				except Exception as e:
					print e
					pass
