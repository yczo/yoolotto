# # GAMES = {
# #         "Powerball": {
# #             "game": "Powerball",
# #             "components": {
# #                 "Powerball": ["Powerball", "Powerball"]
# #             }
# #         },
# #         "MegaMillions": {
# #             "game": "Mega Millions",
# #             "components": {
# #                 "MEGA Millions": ["MegaMillions", "MegaMillions"],
# #                 "Megaplier": ["Megaplier", "Megaplier"]
# #             }
# #         },
# #         "Lotto": {
# #             "game": "Lotto Texas",
# #             "components": {
# #                 "Lotto": ["Lotto", "LottoTexas"]
# #             }
# #         },
# #         "DailyFour": {
# #             "game": "Daily Four",
# #             "components": {
# #                 "Daily 4 Day": ["DailyFour", "DailyFourDay"],
# #                 "Daily 4 Night": ["DailyFour", "DailyFourNight"],
# #                 "Daily 4 Morning": ["DailyFour", "DailyFourMorning"],
# #                 "Daily 4 Evening": ["DailyFour", "DailyFourEvening"]
# #             }
# #         },
# #         "PickThree": {
# #             "game": "Pick Three",
# #             "components": {
# #                 "Pick 3 Day": ["PickThree", "PickThreeDay"],
# #                 "Pick 3 Night": ["PickThree", "PickThreeNight"],
# #                 "Pick 3 Morning":  ["PickThree", "PickThreeMorning"],
# #                 "Pick 3 Evening": ["PickThree", "PickThreeEvening"]
# #             }
# #         },
# #         "CashFive": {
# #             "game": "Cash Five",
# #             "components": {
# #                 "Cash 5": ["CashFive", "CashFive"]
# #             }
# #         },
# #         "TwoStep": {
# #             "game": "Two Step",
# #             "components": {
# #                 "Two Step": ["TwoStep", "TwoStep"]
# #             }
# #         },
# #         "AllOrNothing": {
# #             "game": "All or Nothing",
# #             "components": {
# #                 "All or Nothing Day": ["AllOrNothing", "AllOrNothingDay"],
# #                 "All or Nothing Night": ["AllOrNothing", "AllOrNothingNight"],
# #                 "All or Nothing Morning":  ["AllOrNothing", "AllOrNothingMorning"],
# #                 "All or Nothing Evening": ["AllOrNothing", "AllOrNothingEvening"]
# #             }
# #         },
# #     }
# # for k, v in GAMES.iteritems():
# #     print k,v
# #     for x in v["components"].iteritems():
# #         print x
# # from yoolotto.lottery.feed.draw_common_importer import LotteryTicketCommonImporter,\
# #     LotteryCommonDrawImporter
# # from yoolotto.lottery.feed.data import LotteryTicketDataProvider
# # from yoolotto.lottery.feed.draw_common_importer import LotteryCommonDrawImporter,\
# #     CommonDrawDataProvider
# # from yoolotto.lottery.feed.winning_common_importer import LotteryCommonWinningImporter,\
# #     CommonWinningDataProvider
# from yoolotto.lottery.feed.draw_common_importer import LotteryCommonDrawImporter,\
#     CommonDrawDataProvider
# from yoolotto.lottery.feed.winning_common_importer import LotteryCommonWinningImporter,\
#     CommonWinningDataProvider
# 
# ALL_GAMES = {
#              
#         #--------------------------------global game----------------------------------
#         "Powerball": {
#             "game": "Powerball",
#             "components": {
#                 "Powerball": ["Powerball", "Powerball",True,"US"]
#             },
#             "common_states":[("TX","Texas"),
#                              ("NY","New York"),
#                              ("AR","Arkansas"),
#                              ("AZ","Arizona"),
#                              ("CT","Connecticut"),
#                              ("DC","Dist. of Columbia"),
#                              ("DE","Delaware"),
#                              ("FL","Florida"),
#                              ("GA","Georgia"),
#                              ("IA","Iowa"),
#                              ("ID","Idaho"),
#                              ("IL","Illinois"),
#                              ("IN","Indiana"),
#                              ("KS","Kansas"),
#                              ("KY","Kentucky"),
#                              ("LA","Louisiana"),
#                              ("MA","Massachusetts"),
#                              ("MD","Maryland"),
#                              ("ME","Maine"),
#                              ("MI","Michigan"),
#                              ("MO","Missouri"),
#                              ("NC","North Carolina"),
#                              ("ND","North Dakota"),
#                              ("NE","Nebraska"),
#                              ("NH","New Hampshire"),
#                              ("NJ","New Jersey"),
#                              ("NM","New Mexico"),
#                              ("OH","Ohio"),
#                              ("OK","Oklahoma"),
#                              ("OR","Oregon"),
#                              ("PA","Pennsylvania"),
#                              ("SC","South Carolina"),
#                              ("SD","South Dakota"),
#                              ("TN","Tennessee"),
#                              ("VA","Virginia"),
#                              ("VT","Vermont"),
#                              ("WA","Washington"),
#                              ("WI","Wisconsin"),
#                              ("WV","West Virginia"),
#                              ("RI","Rhode Island"),
#                              ("MN","Minnesota"),
#                              ("MT","Montana"),
#                              ("CO","Colorado ")
#                              #("",""),
#                              ]
#         },
#         
#         "MegaMillions": {
#             "game": "Mega Millions",
#             "components": {
#                 "MEGA Millions": ["MegaMillions", "MegaMillions",True,"US"],
#                 "Megaplier": ["Megaplier", "Megaplier",False,""]
#             },
#             "common_states":[("TX","Texas"),
#                              ("NY","New York"),
#                              ("AR","Arkansas"),
#                              ("AZ","Arizona"),
#                              ("CT","Connecticut"),
#                              ("DC","Dist. of Columbia"),
#                              ("DE","Delaware"),
#                              ("FL","Florida"),
#                              ("GA","Georgia"),
#                              ("IA","Iowa"),
#                              ("ID","Idaho"),
#                              ("IL","Illinois"),
#                              ("IN","Indiana"),
#                              ("KS","Kansas"),
#                              ("KY","Kentucky"),
#                              ("LA","Louisiana"),
#                              ("MA","Massachusetts"),
#                              ("MD","Maryland"),
#                              ("ME","Maine"),
#                              ("MI","Michigan"),
#                              ("MO","Missouri"),
#                              ("NC","North Carolina"),
#                              ("ND","North Dakota"),
#                              ("NE","Nebraska"),
#                              ("NH","New Hampshire"),
#                              ("NJ","New Jersey"),
#                              ("NM","New Mexico"),
#                              ("OH","Ohio"),
#                              ("OK","Oklahoma"),
#                              ("OR","Oregon"),
#                              ("PA","Pennsylvania"),
#                              ("SC","South Carolina"),
#                              ("SD","South Dakota"),
#                              ("TN","Tennessee"),
#                              ("VA","Virginia"),
#                              ("VT","Vermont"),
#                              ("WA","Washington"),
#                              ("WI","Wisconsin"),
#                              ("WV","West Virginia"),
#                              ("RI","Rhode Island"),
#                              ("MN","Minnesota"),
#                              ("MT","Montana"),
#                              ("CO","Colorado ")
#                              #("",""),
#                              ]
#         },
#         
#         #-----------------------------CA------------------------------------
#         
#         "FantasyFive": {
#             "game": "Fantasy Five",
#             "components": {
#                 "Fantasy 5": ["FantasyFive", "FantasyFive",True,""]
#             },
#             "common_states":[("CA","California")]
#         },
#         "DailyFor": {
#             "game": "Daily Four",
#             "components": {
#                 "Daily 4": ["DailyFour", "DailyFour",True,""],
#             },
#             "common_states":[("CA","California")]
#         },
#         "DailyThree": {
#             "game": "Daily Three",
#             "components": {
#                 "Daily 3 Midday": ["DailyThree", "DailyThreeMidDay",True,""],
#                 "Daily 3 Evening": ["DailyThree", "DailyThreeEvening",True,""]
#             },
#             "common_states":[("CA","California")]
#         },
#         "DailyDerby": {
#             "game": "Daily Derby",
#             "components": {
#                 "Daily Derby": ["DailyDerby", "Daily Derby",True,""]
#             },
#             "common_states":[("CA","California")]
#         },
#         "SuperLottoPlus": {
#             "game": "SuperLotto Plus",
#             "components": {
#                 "SuperLotto Plus": ["SuperLottoPlus", "SuperLottoPlus",True,""]
#             },
#             "common_states":[("CA","California")]
#         },
#         "PowerballCA": {
#             "game": "Powerball",
#             "components": {
#                 "Powerball": ["Powerballca", "Powerballca",True,""]
#             },
#             "common_states":[("CA","California")]
#         },
#         "MegaMillionsCA": {
#             "game": "MEGA Millions",
#             "components": {
#                 "MEGA Millions": ["MegaMillionsCA", "MegaMillionsCA",True,""]
#             },
#             "common_states":[("CA","California")]
#         },
#              
#         #----------------------------------------------------------TX----------------------------------
#         "Lotto": {
#             "game": "Lotto Texas",
#             "components": {
#                 "Lotto": ["Lotto", "LottoTexas",True,""]
#             },
#             "common_states":[("TX","Texas")]
#         },
#         "DailyFour": {
#             "game": "Daily Four",
#             "components": {
#                 "Daily 4 Day": ["DailyFour", "DailyFourDay",True,""],
#                 "Daily 4 Night": ["DailyFour", "DailyFourNight",True,""],
#                 "Daily 4 Morning": ["DailyFour", "DailyFourMorning",True,""],
#                 "Daily 4 Evening": ["DailyFour", "DailyFourEvening",True,""]
#             },
#             "common_states":[("TX","Texas")]
#         },
#         "PickThree": {
#             "game": "Pick Three",
#             "components": {
#                 "Pick 3 Day": ["PickThree", "PickThreeDay",True,""],
#                 "Pick 3 Night": ["PickThree", "PickThreeNight",True,""],
#                 "Pick 3 Morning":  ["PickThree", "PickThreeMorning",True,""],
#                 "Pick 3 Evening": ["PickThree", "PickThreeEvening",True,""]
#             },
#             "common_states":[("TX","Texas")]
#         },
#         "CashFive": {
#             "game": "Cash Five",
#             "components": {
#                 "Cash 5": ["CashFive", "CashFive",True,""]
#             },
#             "common_states":[("TX","Texas")]
#         },
#         "TwoStep": {
#             "game": "Two Step",
#             "components": {
#                 "Two Step": ["TwoStep", "TwoStep",True,""]
#             },
#             "common_states":[("TX","Texas")]
#         },
#         "AllOrNothing": {
#             "game": "All or Nothing",
#             "components": {
#                 "All or Nothing Day": ["AllOrNothing", "AllOrNothingDay",True,""],
#                 "All or Nothing Night": ["AllOrNothing", "AllOrNothingNight",True,""],
#                 "All or Nothing Morning":  ["AllOrNothing", "AllOrNothingMorning",True,""],
#                 "All or Nothing Evening": ["AllOrNothing", "AllOrNothingEvening",True,""]
#             },
#             "common_states":[("TX","Texas")]
#         }
#         
#     }
#  
# # def _filter_game(name):
# #         for k, v in COMMON_GAMES.iteritems():
# #             for _name, format in v["components"].iteritems():
# #                 if _name == name:
# #                     return {
# #                         "game_name": v["game"], # Verbose Game Name
# #                         "comp_name": _name,     # Verbose Component Name (e.g. game_name in data)
# #                         "game_format": k,       # Game Code
# #                         "comp_format": format[0],   # Component Name
# #                         "comp_identifier": format[1]
# #                     }
# #             
# #         return None
# # #     
# # import urllib2
# # req = urllib2.Request("http://www.lotterynumbersxml.com/lotterydata/yoolotto.com/jkdladfasdf/lottery.xml")
# # p = urllib2.urlopen(req, timeout=15)# #     
# # import urllib2
# # req = urllib2.Request("http://www.lotterynumbersxml.com/lotterydata/yoolotto.com/jkdladfasdf/lottery.xml")
# # p = urllib2.urlopen(req, timeout=15)
# # xml=p.read()
# # 
# # # #print xml
# # from xml.etree import ElementTree
# # doc=ElementTree.fromstring(xml)
# # xml=p.read()
# # 
# # # #print xml
# # from xml.etree import ElementTree
# # doc=ElementTree.fromstring(xml)
# 
# 
# 
# 
# # print s.get("stateprov_name")
# # print s.get("stateprov_id")
# # found_game=s.find(".//*[@game_name='Powerball']")
# # print found_game.get("game_name")
# 
# 
# # 
# # for k,v in ALL_GAMES.iteritems():
# #     print "    Parent :%s" %k
# #     for component_name,_setting in v["components"].iteritems():
# #         print "        Component :%s" %component_name
# #         for remote_id,current_state in v['common_states']:
# #             print "            state %s - %s" %(remote_id,current_state)
# #             found_state=doc.find(".//*[@stateprov_name='"+current_state+"']")
# #             if found_state is not None:
# #                 #print "            sss>>>>",found_state
# #                 found_component_game=found_state.findall(".//*[@game_name='"+component_name+"']")
# #                 print "            ccc>>>>",found_component_game,component_name
# 
# #for k,v in COMMON_GAMES.iteritems():
# #     print "Picked Game Parent:",k
# #     #self.informer(self.name,"picked up game:%s\n" %k,log=True)
# #     for _name, format in v["components"].iteritems():
# #         print "    Picked Game Component:",_name
# #         for current_state in v['common_states']:
# #             found_state=doc.find(".//*[@stateprov_name='"+current_state+"']")
# #             print "        found state",found_state.attrib
# #             if found_state:
# #                 #self.informer(self.name,"    state found :%s\n" %current_state,log=True)
# #                 
# #                 #self.log_file.write("Found Game %s\n" % game_meta)
# #                 
# #     #             division, _ = LotteryCountryDivision.objects.get_or_create(
# #     #                                                                        name=found_state.get("stateprov_name"),
# #     #                                                                        remote_id=found_state.get("stateprov_id"),
# #     #                                                                        remote_country=found_state.get("country")
# #     #                                                                        )
# #     #             division.save()
# #                 found_game=found_state.find(".//*[@game_name='"+_name+"']")
# #                 print "        found game ",found_game.attrib
# # #cs=[("TX"),("AR"),("AZ")]
# # 
# # # for state in xml.findall("StateProv"):
# # #     if state.get("stateprov_name") in  
# # #                 continue
# # # from termcolor import colored
# # # def informer(_type,msg,debug=True,log=False,color='green',type_color="cyan"):
# # #         if debug :print '[ '+colored(_type,type_color)+' ]'" :"+colored(msg,color)
# # # #         if log:
# # # #             try:
# # # #                 self.log_file.write(msg)
# # # #             except Exception,e:
# # # #                 print colored(e,'red','on_white')
# # #                 
# # #                 
# # # informer('impoter', "starting..")
# 
# 
# # x=LotteryCommonDrawImporter(CommonDrawDataProvider.remote)
# # x.run()
# 
# # x=LotteryCommonWinningImporter(CommonWinningDataProvider.sample)
# # x.run()
# 
# # import pystmark
# # message = pystmark.Message(sender="alert@yoolotto.com",
# #                            to='irfan@spa-systems.com',
# #                            subject='IMPORTER ALERT !!!',
# #                            text='A message')
# # state=pystmark.send(message, api_key="bab5362e-a9d2-4dfe-974c-8fc95bf93b79")
# # state.raise_for_status()
# # print "DONE"
# 
# 
# # from django.core.mail import send_mail
# # send_mail('IMPORTER ALERT', 'Here is the message.', 'spa.backend@gmail.com',
# #     ['irfan@spa-systems.com'], fail_silently=False)
# # print "done"
# 

 
# from yoolotto.lottery.feed.draw_common_importer import LotteryCommonDrawImporter,\
#     CommonDrawDataProvider
# lcdi=LotteryCommonDrawImporter(CommonDrawDataProvider.remote)
# lcdi.run()
# from yoolotto.lottery.feed.draw_common_importer import LotteryCommonDrawImporter,\
#      CommonDrawDataProvider
# from yoolotto.lottery.feed.winning_common_importer import LotteryCommonWinningImporter,\
#     CommonWinningDataProvider
# lcdi=LotteryCommonDrawImporter(CommonDrawDataProvider.remote)
# lcdi.run()
# lcwi=LotteryCommonWinningImporter(CommonWinningDataProvider.remote)
# lcwi.run()

#2014-06-04 13:53:17.273057
# 
# state="DCC"
# old_states=["CA","TX","DC","NY"]
# if (not old_states and state=="TX") or (state in old_states):
#     print "send"
# else:
#     print "skipped !"
#   
  
#
# l=[2,3,4]     
# for i,x in enumerate(l):
#     print i,x

# for u in YooLottoUser.objects.all()
# set(YooLottoUser.objects.get(id=user_id).tickets.values_list('division__remote_id',flat=True))




#     #again filter for tickets and state
#     played_for_draw_states=[]
#     [played_for_draw_states.append(s) for s in send_for_states if LotteryTicket.objects.filter(user__id=user_id,draw__id=draw_id,division__remote_id=s)]
#     
#     if played_for_draw_states:
#         send_for_states=played_for_draw_states
#     else:
    