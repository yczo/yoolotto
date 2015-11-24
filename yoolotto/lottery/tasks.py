import random
import datetime

from celery.task import PeriodicTask, task
from celery.schedules import crontab
from yoolotto.util.async import report_errors
from yoolotto.lottery.models import LotteryCountryDivision




class UpdateDraws(PeriodicTask):
    run_every = datetime.timedelta(minutes=30)
    @report_errors
    def run(self, **kwargs):
        from yoolotto.lottery.feed.draw_common_importer import LotteryCommonDrawImporter,\
        CommonDrawDataProvider
        from yoolotto.lottery.feed.winning_common_importer import LotteryCommonWinningImporter,\
        CommonWinningDataProvider

        lcdi=LotteryCommonDrawImporter(CommonDrawDataProvider.remote)
        lcdi.run()
        lcwi=LotteryCommonWinningImporter(CommonWinningDataProvider.remote)
        lcwi.run()
          
          

#specially taken for sending to iphone, integer instead of remote_id 
state_codes={
             "TX":0,
             "CA":2,
             "AZ":3,
             "AR":4,
             "CO":5,
             "CT":6,
             "DE":7,
             "DC":8,
             "FL":9,
             "GA":10,
             "ID":11,
             "IL":12,
             "IN":13,
             "IA":14,
             "KS":15,
             "KY":16,
             "LA":17,
             "ME":18,
             "MD":19,
             "MA":20,
             "MI":21,
             "MN":22,
             "MO":23,
             "MT":24,
             "NE":25,
             "NH":26,
             "NJ":27,
             "NM":28,
             "NY":29,
             "NC":30,
             "ND":31,
             "OH":32,
             "OK":33,
             "OR":34,
             "PA":35,
             "RI":36,
             "SC":37,
             "SD":38,
             "TN":39,
             "VT":40,
             "VA":41,
             "WA":42,
             "WI":43,
             "WV":44,
	     "WY":45
             }
    
@task
@report_errors
def notify_draw_result_all(draw_id):
    from yoolotto.lottery.models import LotteryTicket
    for _id in set([x["pk"] for x in LotteryTicket.objects.filter(
        draw_id=int(draw_id), submissions__checked=False, notified=False).values("pk")]):
        notify_draw_result_for_ticket.apply_async(countdown=random.randrange(30), 
            kwargs={"ticket_id": _id})
        
@task
@report_errors
def notify_draw_result_for_ticket(ticket_id):
    from yoolotto.lottery.models import LotteryTicket
    from yoolotto.communication.apn import APNSender
    from yoolotto.communication.gcm_sender import GCMSender
        
    try:
        ticket = LotteryTicket.objects.get(pk=int(ticket_id))
    except:
        return
    
    if not ticket.user.preferences.jackpot_drawing:
        return
    
    log = ""
    log += "\nTicket: " + str(ticket.pk)
    text = "Results available for %s" % ticket.draw.component.name
    log += "\n" + text
    
    for device in ticket.user.devices.all():
        if not device.device_token:
            log += "\nSkipped Device, No Token"
            continue
        
        if device.is_ios():
            apn = APNSender(device.device_token, text=text, 
               custom={"code": "draw", "drawId": ticket.draw.pk,"state":state_codes[ticket.division.remote_id]})
            apn.send()
            log += "\nAPN Outbound: %s" % device.device_token
        elif device.is_android():
            gcm = GCMSender(to=[device.device_token], data={"text": text,
                                                            "code": "draw", "drawId": ticket.draw.pk,"state":ticket.division.remote_id,"ticket_id":ticket.id
                                                            })
            gcm.send()
            log += "\nGCM Outbound: %s" % device.device_token
    
    ticket.notified = True
    ticket.save()
    
    return log


class FrenzyDailyNotification(PeriodicTask):
    run_every = datetime.timedelta(minutes=20)
    #run_every = crontab(minute="5", hour="18")

    @report_errors
    def run(self, **kwargs):
        from yoolotto.lottery.models import LotteryDraw
        
        candidates = LotteryDraw.objects.filter(
            jackpot__gte=75000000, official=True, result__isnull=True,
            date__gte=datetime.date.today())
        
        print "Candidates:", candidates
        
        for candidate in candidates:
            frenzy = candidate.frenzies.filter(
                added_at__gte=datetime.datetime.now()-datetime.timedelta(hours=18))
            
            if frenzy:
                continue
            
            if candidate.date != datetime.date.today():
                continue
            
            notify_frenzy_all.apply_async(countdown=5, kwargs={"draw_id": candidate.pk})
        
@task
@report_errors
def notify_frenzy_all(draw_id):
    from yoolotto.lottery.enumerations import EnumerationManager
    from yoolotto.lottery.models import LotteryDraw, LotteryDrawFrenzy
    from yoolotto.user.models import YooLottoUser
    
    draw = LotteryDraw.objects.get(pk=int(draw_id))
    
    if draw.jackpot < 75000000:
        return
    
    draw.frenzied = True
    draw.save()
    
    frenzy = LotteryDrawFrenzy(draw=draw)
    frenzy.save()
    
#    print "TESTING", draw, frenzy
#    return
    
    component_name = draw.component.name
    draw_value = "{:,}".format(draw.jackpot)
    game_type = EnumerationManager.game_reverse(draw.component.parent.pk)
    
    for user in YooLottoUser.objects.filter(preferences__jackpot_frenzy=True).values("pk"):
        notify_frenzy_for_user.apply_async(countdown=random.randrange(60), 
            kwargs={"component_name": component_name, "draw_value": draw_value, 
                "game_type": game_type, "user_id": user["pk"],"draw_id":draw_id})
        
@task
@report_errors
def notify_frenzy_for_user(component_name, draw_value, game_type,user_id,draw_id):
    from yoolotto.lottery.models import LotteryDraw
    from yoolotto.user.models import YooLottoUser
    from yoolotto.communication.apn import APNSender
    from yoolotto.communication.gcm_sender import GCMSender
    
    state=LotteryDraw.objects.get(id=draw_id).division.remote_id
    # reducing multiple frenzies notification for any user , 
    # based on user old hits for states in history
    # and also sending duplicate notification for state TX and game_type in 0/1 Megamillion/Powerball
    old_states=[str(_state) for _state in set(YooLottoUser.objects.get(id=user_id).tickets.values_list('division__remote_id',flat=True))]
    send_for_states=[]
    
    log=""
    
    if (not old_states and state=="TX"):
        send_for_states.append("TX")
    elif (state in old_states):
        send_for_states.append(state)
    else:
        log += "\nDevice auto skipped ,assume unnecessary state: %s" % state
    
    if old_states and state=="TX" and game_type in [0,1]: # 0/Megamillion(TX),1/Powerball(TX):
        #send duplicate notification for state that user has already played
        [send_for_states.append(i) for i in old_states if i!="CA"]
        
        
    send_for_states=list(set(send_for_states))
    for _index,right_state in enumerate(send_for_states):#loop for duplicate notification
        if right_state!="TX" and len(send_for_states) > 1:
            log += "\n Send duplicate notification for state: %s" % right_state
            
        user = YooLottoUser.objects.get(pk=int(user_id))
        log += "\nUser: " + str(user.pk)
        state_name=LotteryCountryDivision.objects.get(remote_id=right_state).name.title()
        text = "%s %s Jackpot is %s!" % (state_name,component_name, draw_value)
        
        log += "\n" + text
        
        
        for device in user.devices.all():
            if not device.device_token:
                log += "\nSkipped Device, No Token"
                continue
            
            if device.is_ios():
                apn = APNSender(device.device_token, text=text,
                                custom={"code": "frenzy", "gameType": game_type,
                                        "state":state_codes[right_state],
                                        "drawId":draw_id
                                        })
                apn.send()
                log += "\nAPN Outbound: %s" % device.device_token
            elif device.is_android():
                gcm = GCMSender(to=[device.device_token], data={"text": text,
                                                                "code": "frenzy", "gameType": game_type,
                                                                "state":right_state,
                                                                "draw_id":draw_id+_index
                                                                })
                gcm.send()
                log += "\nGCM Outbound: %s" % device.device_token
        
        
        log += "\n ---------------------------- \n"
       
        
    return log
    
   
        
    
    
    
    
    
