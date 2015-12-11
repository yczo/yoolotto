from ast import literal_eval
import ast
import datetime
import os
from xml.etree import ElementTree
from django.conf import settings
from termcolor import colored
from yoolotto.lottery.game.manager import GameManager
from yoolotto.lottery.models import LotteryCountryDivision, LotteryGame, LotteryGameComponent, LotteryDraw

import urllib2
from yoolotto.lottery.feed import importer_switch_board as IMPORTER_SETTINGS 
from yoolotto.lottery.tasks import notify_draw_result_all
#from yoolotto.lottery.tasks import notify_draw_result_all


class CommonDrawDataProvider(object):
    """
    common data provider for draws
    """
    PROVIDER_URL = "http://www.lotterynumbersxml.com/lotterydata/yoolotto.com/jkdladfasdf/lottery.xml"
    
    @staticmethod
    def sample():
        path = os.path.join(settings.SITE_ROOT, "???????")
        fp = open(path)
        result = fp.read()
        fp.close()
        
        return result
    
    @staticmethod
    def remote():
        req = urllib2.Request(CommonDrawDataProvider.PROVIDER_URL)
        p = urllib2.urlopen(req, timeout=15)
        
        if p:
            return p.read()

        
class LotteryCommonDrawImporter(object):
    
    log_file=None
    log_draw_folder="draw"
    
    """
    will be served as common importer for games played in many states and have same rules
    NOTE: in draw update xml game_name is component name.
    
    """
    
    def __init__(self, provider):
        self._provider = provider
        self.COMMON_GAMES=IMPORTER_SETTINGS.ALL_GAMES
        self.name=self.__class__.__name__
        
        
    def informer(self,_type,msg,debug=True,log=False,color='green',type_color="cyan",attrs=["bold"]):
        if debug :print '[ '+colored(_type,type_color,attrs=attrs)+' ]'" :"+colored(msg,color,attrs=attrs)
        if log:
            try:
                self.log_file.write(msg)
            except Exception,e:
                print colored(e,'red','on_white',attrs=['bold'])
    
    def _filter_game(self, comp_name,parent_code):
        for k, v in self.COMMON_GAMES.iteritems():
            for _name,_format in v["components"].iteritems():
                if _name == comp_name and k==parent_code:
                    return {
                        "game_name": v["game"], # Parent Game Name
                        "comp_name": _name,     # Component Name (e.g. game_name in data)
                        "game_format": k,       # Parent Game Code
                        "comp_format":_format[0],   # Component Format
                        "comp_identifier":_format[1] #Component identifier
                    }
            
        return None
        
    def run(self):
        
        # Debug / Logging of Imports
        log_path=os.path.join(settings.IMPORT_LOG_PATH,self.log_draw_folder)
        if not os.path.exists(log_path):
            os.mkdir(log_path)
            
        path = os.path.join(log_path,datetime.datetime.now().strftime("draw-%Y-%m-%dT%H-%M-%S"))
        if not os.path.exists(path):
            os.mkdir(path)
            
        file_dump = os.path.join(path, "common_draw_import.xml")
        file_log = os.path.join(path, "common_draw_import.log")
                
        data = self._provider()
        
        fp_dump = open(file_dump, "w")
        self.log_file = open(file_log, "w")
        
        fp_dump.write(data)
        fp_dump.close()
        
        # Start Import
        xml = ElementTree.fromstring(data)
          
        self.informer(self.name,"started :"+str(datetime.datetime.now().strftime("%d/%m/%Y %H:%M:%S"))+" \n",log=True)
        for k,v in self.COMMON_GAMES.iteritems():
            self.informer(self.name,"    Picked Game Parent :%s\n" %k,log=True)
            for component_name,_setting in v["components"].iteritems():
                self.informer(self.name,"        Picked Game Component :%s\n" %component_name,log=True)
                for remote_id,current_state in v['common_states']:
                    if component_name=="Megaplier" and remote_id not in ["TX"]:
                        #skip Megaplier for all states except (TX) 
                        continue
                    
                    found_state=xml.find(".//*[@stateprov_name='"+current_state+"']")
                    if found_state is not None:
                        self.informer(self.name,"            fill for state :%s\n" %current_state,log=True)
                        
                        #self.log_file.write("Found Game %s\n" % game_meta)
#                         print ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
#                         print found_state.get("stateprov_name").strip()
#                         print found_state.get("stateprov_id").strip()
#                         print found_state.get("country").strip()
#                         print "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
                        division, _ = LotteryCountryDivision.objects.get_or_create(
                                                                                   name__iexact=found_state.get("stateprov_name").strip(),
                                                                                   remote_id__iexact=found_state.get("stateprov_id").strip(),
                                                                                   remote_country__iexact=found_state.get("country").strip()
                                                                                   )
                        division.save()
                        if _:self.informer(self.name,"            created division :%s\n" %division,color="cyan",log=True)
                        else:
                            self.informer(self.name,"            get division :%s\n" %division,log=True)
                                
                        found_component_game=found_state.find(".//*[@game_name='"+component_name+"']")
                        if found_component_game is not None:
                            self.informer(self.name,"            Is valid component for state ? :%s\n" %True,log=True)
                            component_meta = self._filter_game(comp_name=found_component_game.get("game_name"),parent_code=k)#get game component name
                            if component_meta:
                                self.informer(self.name,"            Has component meta ? :%s\n" %True,log=True)                          
                                
                                parent_game,_= LotteryGame.objects.get_or_create(
                                            name=component_meta["game_name"],
                                            code=component_meta["game_format"]
                                            )
                                
                                if _:self.informer(self.name,"            created new parent :%s\n" %parent_game,color="cyan",log=True)
                                else:
                                    self.informer(self.name,"            got component parent :%s\n" %parent_game,log=True)
                                component,_ = LotteryGameComponent.objects.get_or_create(
                                                remote_id=found_component_game.get("game_id"),
                                                name=component_meta["comp_name"],
                                                format=component_meta["comp_format"]
                                                )
                                
                                if _:self.informer(self.name,"            created new component :%s\n" %component,color="cyan",log=True)
                                else:
                                    self.informer(self.name,"            got component :%s\n" %component,log=True)
                                
                                component.parent = parent_game
                                component.division.add(division)
                                component.format = component_meta["comp_format"]
                                component.identifier = component_meta["comp_identifier"]
                                component.save()
                                
                                handler = GameManager.get(component_meta["game_format"])#common handler for all types of component for single game 
                                self.informer(self.name,"            handler picked up :%s\n" %handler,log=True)
                                
                                # Load Jackpot
                                jackpot = found_component_game.find("jackpot")
                                jackpot_date = None
                                jackpot_prize = None
                                                
                                if jackpot is not None:
                                    jackpot_date = datetime.datetime.strptime(jackpot.get("date").strip(), "%m/%d/%Y").date()
                                    jackpot_prize = int(jackpot.text)
                                    self.informer(self.name,"            Found Jackpot tag with prize (%s) and attrib date (%s) \n" %(jackpot_prize,jackpot_date),log=True)
                                else:
                                    self.informer(self.name,"            Jackpot tag not found.(assume normal) \n",color="yellow",log=True)
                                
                                # Load Last Draw
                                last_draw_date = found_component_game.find("lastdraw_date")
                                last_draw_numbers = found_component_game.find("lastdraw_numbers")
                                self.informer(self.name,"            result available (%s) for last draw date (%s). \n" %(last_draw_numbers.text,last_draw_date.text),log=True)
                                
                                
                                if last_draw_date is not None and last_draw_numbers is not None:
                                    last_draw_date = datetime.datetime.strptime(last_draw_date.text, "%m/%d/%Y").date()
                                    
                                    if component_meta["game_name"] in ["All or Nothing"] and not jackpot:
                                        jackpot_prize = 250000
                                        jackpot_date = last_draw_date
                                        self.informer(self.name,"          handler picked up :<cl  Assume fix jackpot prize of (%s). \n" %(250000),color="yellow",log=True)
                                    
                                    _notification = False
                                    
                                    # Find Draw Object
                                    
                                    try:
                                        draw, created = \
                                        LotteryDraw.objects.get_or_create(component=component,
                                                                         date=last_draw_date,
                                                                         division_id=division.id)
                                    except Exception as e:
                                        self.informer(self.name,"            exception in retrieving draw for last draw date (%s) ,exception :%s \n"%(last_draw_date,e),color="red",log=True)
                                    
                                    if created:self.informer(self.name,"            created LotteryDraw id=(%s) for last draw date (%s)\n"%(draw.id,last_draw_date),color="yellow",log=True)
                                    else:
                                        self.informer(self.name,"            draw already exist LotteryDraw id=(%s) of last draw date (%s)\n"%(draw.id,last_draw_date),log=True)
                                    
                                    
                                    if not draw.result:
                                        _notification = True
                                        self.informer(self.name,"            prepared for draw result available notification for date (%s) \n"%(last_draw_date),log=True)
                                    draw.result = handler.decode(last_draw_numbers.text,format=component_meta["comp_format"])
                                    
                                    #daily derby racetime has came through jackpot xml not from winning xml
                                    # filling race time for component has name=="Daily Derby" and (if future daily derby has more than one component)identifier=="Daily Derby"  
                                    if component_name=="Daily Derby" and component_meta.get("comp_identifier")=="Daily Derby":
                                        try:
                                            draw.race_time = handler.decode_race_time(last_draw_numbers.text, format=component_meta["comp_format"])
                                            draw.save()
                                        except Exception as e:
                                            self.informer(self.name,"            ( Daily Derby ) race time not filled for draw id = (%s) \n"%(draw.id),color="red",log=True)
                                    if component_meta["game_name"] == "Powerball":
                                        if len(literal_eval(draw.result)) != 7:
                                            raise ValueError("Unexpected Result Value for powerball: %s" % draw.result)
                                        else:
                                            draw.powerplay = literal_eval(draw.result)[-1]
                                    if component_meta["game_name"] == "MegaMillions":
				        if len(literal_eval(draw.result))!=7:
					    raise ValueError("Unexpected Result Value fo Megamillion: %s"% draw.result)
					else:
					    draw.megaplier = literal_eval(draw.result)[-1]
                                    #If the draw object was created OR the draw became official, 
                                    #fire a notification for Draw Results
                                    if created or not draw.official:
                                        _notification = True
                                     
                                    draw.official = True
                                     
                                    #if jackpot is old update old draw jackpot
                                    if jackpot_prize is not None and jackpot_date == last_draw_date:
                                        draw.jackpot = jackpot_prize
                                        self.informer(self.name,"            found old jackpot of last draw date (%s) \n"%(last_draw_date),color="yellow",log=True)
                                     
                                    draw.save()#valid data hasn't store yet. 
                                    self.informer(self.name,"        draw saveddd --------------------------- %s"%draw.result)
                                    #search for powerplay and create it
                                    if component_meta["game_name"] == "Powerball" and len(ast.literal_eval(draw.result)) == 7 and remote_id=="TX":
                                        #For last_draw_date
                 
                                        powerplay = ast.literal_eval(draw.result)[6:7]
                                        self.informer(self.name,"            found powerplay :%s\n"%(powerplay),log=True)
                                        
                                        # first create a fake powerplay game component
                                        
                                        powerplay_component, _ = LotteryGameComponent.objects.get_or_create(
                                                        remote_id=found_component_game.get("game_id").strip(),#powerplay has same remote_id as powerball
                                                        name="Powerplay"
                                                        )
                                        powerplay_component.parent = parent_game
                                        powerplay_component.division.add(division)
                                        powerplay_component.format = "Powerplay"
                                        powerplay_component.identifier = "Powerplay"
                                        powerplay_component.save()
                                        
                                        #creating draw object for Powerplay
                                        powerplay_draw, powerplay_created = LotteryDraw.objects.get_or_create(
                                                                component=powerplay_component,
                                                                date=last_draw_date,
                                                                division_id=division.id,
                                                                result=powerplay
                                                                )
                                        if powerplay_created:
                                            self.informer(self.name,"            powerplay draw (id=%s) created for existing last draw date :%s\n"%(powerplay_draw.id,last_draw_date),log=True)
                                        
                                            
                                        #If the draw object was created OR the draw became official, 
                                        #fire a notification for Draw Results
                                        if powerplay_created or not powerplay_draw.official:
                                            _notification = True
                                            self.informer(self.name,"            (powerplay)prepared for draw result available notification for date (%s) \n"%last_draw_date,log=True)
                 
                                        powerplay_draw.official = True
                                        powerplay_draw.save()
                                        self.informer(self.name,"            powerplay draw (id=%s) updated for existing last draw date :%s\n"%(powerplay_draw.id,last_draw_date),log=True)
                                        
                                    
                                        
                                        #Finally Storing the draw result without powerplay
                                        draw.result = str(ast.literal_eval(draw.result)[0:6])
                                        draw.save()
                                        self.informer(self.name,"            powerball draw result saved (draw id=%s) \n"%draw.id,log=True)		    
                                    if component_meta["game_name"] == "MegaMillions" and len(ast.literal_eval(draw.result)) == 7 and remote_id=="TX":
                                    #For last_draw_date
                                        megaplier = ast.literal_eval(draw.result)[6:7]
                                        self.informer(self.name,"            found megaplier :%s\n"%(megaplier),log=True)
                                        # first create a fake powerplay game component
                                        megaplier_component, created = LotteryGameComponent.objects.get_or_create(remote_id=found_component_game.get("game_id").strip(),name="Megaplier")
                                        megaplier_component.parent = parent_game
                                        megaplier_component.division.add(division)
                                        megaplier_component.format = "Megaplier"
                                        megaplier_component.identifier = "Megaplier"
                                        megaplier_component.save()
                                        #creating draw object for Powerplay
                                        try:
                                            megaplier_draw, powerplay_created = LotteryDraw.objects.get_or_create(component=megaplier_component,date=last_draw_date,division_id=division.id,result=megaplier)
                                        except:
                                            megaplay_draw = LotteryDraw.objects.get_or_create(component=megaplier_component,date=last_draw_date,division_id=division.id,result=megaplier)
                                            megaplier_created = False
                                        if megaplier_created:
                                            self.informer(self.name,"            megaplier draw (id=%s) created for existing last draw date :%s\n"%(megaplier_draw.id,last_draw_date),log=True)
                                        #If the draw object was created OR the draw became official, 
                                        #fire a notification for Draw Results
                                        if megaplier_created or not megaplier_draw.official:
                                            _notification = True
                                            self.informer(self.name,"            (megaplier)prepared for draw result available notification for date (%s) \n"%last_draw_date,log=True)
                                        megaplier_draw.official = True
                                        megaplier_draw.save()
                                        self.informer(self.name,"            megaplier draw (id=%s) updated for existing last draw date :%s\n"%(megaplier_draw.id,last_draw_date),log=True)
                                            #Finally Storing the draw result without powerplay
                                        draw.result = str(ast.literal_eval(draw.result)[0:6])
                                        draw.save()
                                        self.informer(self.name,"            megamillions draw result saved (draw id=%s) \n"%draw.id,log=True)

                                    #set other states result for powerball to 6 length
                                    if component_meta["game_name"] == "Powerball" and len(ast.literal_eval(draw.result)) == 7:
                                        draw.result = str(ast.literal_eval(draw.result)[0:6])
                                        draw.save()
				                    if component_meta["game_name"] == "MegaMillions" and len(ast.literal_eval(draw.result)) == 7:
                                        draw.result = str(ast.literal_eval(draw.result)[0:6])
                                        draw.save()
                                    if _notification:
                                        try:
                                            self.informer(self.name,"            started sending draw result notification for old draw (id=%s) \n"%draw.id,log=True)
                                            notify_draw_result_all.apply_async(countdown=0,
                                                                           kwargs={"draw_id": draw.pk})
                                            
                                        except Exception as e:
                                            self.informer(self.name,"            Exception in sending draw result notification for old draw (id=%s)  :%s\n"%(draw.id,e),color="red",log=True)
                                 
                                
                                
                                # Load Current Draw
                                next_draw_date = found_component_game.find("nextdraw_date")
                                if next_draw_date is not None:
                                    self.informer(self.name,"            Next draw available for date :%s \n"%next_draw_date.text,log=True) 
                                else:
                                    self.informer(self.name,"            Next draw tag missing \n",color="yellow",log=True)
        
                                
                                 
                                if next_draw_date is not None:
                                    next_draw_date = datetime.datetime.strptime(next_draw_date.text, "%m/%d/%Y").date()
                                    _notification = False
                                     
                                    # Find Draw Object
                                    draw, created = LotteryDraw.objects.get_or_create(component=component,
                                                                                     date=next_draw_date,
                                                                                     division_id=division.id)
                                     
                                    
                                    if created:self.informer(self.name,"            created LotteryDraw id=(%s) of next draw date (%s) \n"%(draw.id,next_draw_date),log=True)
                                    else:
                                        self.informer(self.name,"            draw already exist LotteryDraw id=(%s) for next draw date (%s) \n"%(draw.id,next_draw_date),log=True)
                                        
                                    #If the draw object was created OR the draw became official, 
                                    #fire a notification for Draw Results
                                    if created or not draw.official:
                                        _notification = True
                                                    
                                    draw.official = True
                                     
                                    self.informer(self.name,"            prepared for frenzy notification for next draw date (%s) \n"%next_draw_date,log=True)
                                    
                                                                         
                                    if jackpot_date == next_draw_date:
                                        draw.jackpot = jackpot_prize
                                        self.informer(self.name,"            next draw jackpot available :%s\n"%jackpot_prize,log=True)
                                    else:
                                        self.informer(self.name,"            next draw jackpot not filled \n",color="yellow",log=True)
                                        
                                    
                                    if draw.jackpot and _notification:
                                        try:
                                            self.informer(self.name,"            started sending frenzy notification for new draw (id=%s) \n"%draw.id,log=True)
#                                             notify_frenzy_all.apply_async(countdown=300,
#                                                                           kwargs={"draw_id": draw.pk})
                                        except Exception as e:
                                            self.informer(self.name,"            Exception in sending frenzy notification for new draw (id=%s) :%s\n"%(draw.id,e),color="red",log=True)
                                    
                                    
                                    draw.save()
                                    self.informer(self.name,"            next draw saved \n",log=True)
                                    
                                self.informer(self.name,"\n                --------------------------------------------\n",log=True)
                            else:
                                self.informer(self.name,"           (auto skip) Has component meta ? :%s\n" %False,log=True,color="red")
                        else:
                            self.informer(self.name,"           (auto skip) Is valid component for state ? :%s\n" %False,log=True,color="red")
                    else:
                        self.informer(self.name,"            (auto skip) state not found :%s\n" %current_state,log=True,color="red")
                        self.informer(self.name,"\n            --------------------------------------------\n",log=True)
                        #print colored(e,'red','on_white',attrs=['bold'])
            self.informer(self.name,"\n        --------------------------------------------\n",log=True)
        self.informer(self.name,"\n------------------------DONE--------------------\n",log=True)
                    
            
        # Close Logger
        self.log_file.close()




