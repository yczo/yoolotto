import os
import datetime
from xml.etree import ElementTree
from termcolor import colored
from django.conf import settings
from yoolotto.lottery.models import LotteryGameComponent, LotteryCountryDivision
from yoolotto.lottery.models import LotteryDraw
import urllib2
from yoolotto.lottery.feed import importer_switch_board as IMPORTER_SETTINGS 

class CommonWinningDataProvider(object):
    """
    common data provider for draws
    """
    PROVIDER_URL = "http://www.elottonumbers.com/feed/payout/yoolotto.com/kdadljasdf/payout-all.xml"
    
    @staticmethod
    def sample():
        path = os.path.join(settings.SITE_ROOT, "/home/irfanansari/Desktop/payout2.xml")
        fp = open(path)
        result = fp.read()
        fp.close()
        
        return result
    
    @staticmethod
    def remote():
        req = urllib2.Request(CommonWinningDataProvider.PROVIDER_URL)
        p = urllib2.urlopen(req, timeout=15)
        
        if p:
            return p.read()


class LotteryCommonWinningImporter(object):
    log_file=None
    log_winning_folder="winning"
    
    def __init__(self, provider):
        self._provider = provider
        self.name=self.__class__.__name__
        self.COMMON_GAMES=IMPORTER_SETTINGS.ALL_GAMES
    
    def informer(self,_type,msg,debug=True,log=False,color='green',type_color="cyan",attrs=["bold"]):
        if debug :print '[ '+colored(_type,type_color,attrs=attrs)+' ]'" :"+colored(msg,color,attrs=attrs)
        if log:
            try:
                self.log_file.write(msg)
            except Exception,e:
                print colored(e,'red','on_white',attrs=['bold'])
    
    def run(self):
        
        # Debug / Logging of Imports
        log_path=os.path.join(settings.IMPORT_LOG_PATH,self.log_winning_folder)
        if not os.path.exists(log_path):
            os.mkdir(log_path)

        path = os.path.join(log_path,datetime.datetime.now().strftime("winning-%Y-%m-%dT%H-%M-%S"))
        if not os.path.exists(path):
            os.mkdir(path)
            
        file_dump = os.path.join(path, "common_winning_import.xml")
        file_log = os.path.join(path, "common_winning_import.log")
                
        data = self._provider()

        fp_dump=open(file_dump, "w")
        self.log_file=open(file_log, "w")

        fp_dump.write(data)
        fp_dump.close()
        
        xml = ElementTree.fromstring(data)
        self.informer(self.name,"-----------------------started [%s]\n\n"%datetime.datetime.now(),log=True)
        for k,v in self.COMMON_GAMES.iteritems():
            self.informer(self.name,"    Picked Game :%s\n" %k,log=True)
            for component_name,_setting in v["components"].iteritems():
                if _setting[2]:
                    self.informer(self.name,"        Picked Component :%s\n" %component_name,log=True)
                    for state_code,current_state in v['common_states']:
                        self.informer(self.name,"        ---------------------Picked State :%s\n"%current_state)
                        self.informer(self.name,"        search winning for component (%s) ,state(%s) \n" %(component_name,state_code),log=True)
                        game=xml.find(".//winners/..[@game_name='"+component_name+"']"+"[@area='"+state_code+"']")
                        if game is None:
                            self.informer(self.name,"        look in alternative state :%s\n" %_setting[3],color="yellow",log=True)
                            game=xml.find(".//winners/..[@game_name='"+component_name+"']"+"[@area='"+_setting[3]+"']")
                        
                        if game is not None:
                            self.informer(self.name,"        winnings available \n" ,log=True)
                            division=LotteryCountryDivision.objects.get(name=current_state,remote_id=state_code)
                            
                            game_id=game.get('game_id').strip()
                            game_name=game.get('game_name').strip()
                            area=game.get('area').strip()
                            draw_date=game.get('draw_date').strip()
                            updated=game.get('updated').strip()
                            component = \
                            LotteryGameComponent.objects.get(name=game_name,
                                                            remote_id=game_id,
                                                            division__id=division.id
                                                            )
                            self.informer(self.name,"        found LotteryGameCompoent :%s\n"%LotteryGameComponent)
                            
                            try:
                                
                                # Now get the draw object
                                draw = \
                                LotteryDraw.objects.get(date = datetime.datetime.strptime(draw_date, '%Y-%m-%d').date(),
                                                        component=component,
                                                        division_id=division.id
                                                        )

                                self.informer(self.name,"        found LotteryDraw :(id=%s)\n"%draw.id)
                                for child in game:
                                    #assume <match> tag always come in xml 
                                    #findtext(match, default=None)
                                    #Finds text for the first subelement matching match. match may be a tag name or path. Returns the text content of the first matching element, or default if no element was found. Note that if the matching element has no text content an empty string is returned.

                                    match=child.findtext('match').strip()
                                    if child.findtext('prize',default=None) is not None:
                                        prize=child.findtext('prize').strip()
                                    else:
                                        prize=0
                                    
                                    #num_winners=child.find('num_winners').text
                                    #draw.number_of_winners=num_winners
                                    #draw.updated=datetime.datetime.strptime(draw_date, '%Y-%m-%d %H:%M:%S').date()#2014-04-12 00:07:05
                                    
                                    
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
                                    #-------------------------------------------from TX------------
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
                                      
                            except Exception as e:
                                self.informer(self.name,"        updating winnings failed ,exception :%s\n"%e ,color="red",log=True)
                              
                        else:
                            self.informer(self.name,"        winnings not found \n" ,color="red",log=True)
                    
                        self.informer(self.name,"        ----------------------------------- \n",log=True)        
                        
                        
                else:
                    self.informer(self.name,"        component winning is turned off :%s\n" %component_name,color="yellow",log=True)
        self.informer(self.name,"-----------------------done----------------------",log=True)

        
                    
