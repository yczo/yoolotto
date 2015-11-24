import random
from yoolotto.lottery.models import LotteryTicket, LotteryDraw,LotteryCountryDivision
from django.views.generic import View
from django.http.response import HttpResponse
from yoolotto.lottery.tasks import notify_draw_result_all
from yoolotto.lottery.enumerations import EnumerationManager
from yoolotto.user.models import YooLottoUser

'''
Examples:-

(find tickets of draw)
http://IP:PORT/lottery/manual_notification/find_tickets?of_draw=7808


(find user of ticket)
http://IP:PORT/lottery/manual_notification/find_user?of_ticket=12990
http://IP:PORT/lottery/manual_notification/last_user

(send notification for ticket id  directly)
http://IP:PORT/lottery/manual_notification?for_ticket=33370
 OR 
(send notification for all tickets of draw )
http://IP:PORT/lottery/manual_notification?for_draw=7808



'''
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


class ManualNotification(View):
    def get(self,request):
        notification_type=request.GET.get('type','result')
        for_draw=int(request.GET.get('for_draw','0'))
        for_ticket=int(request.GET.get('for_ticket','0'))
        for_user=int(request.GET.get('for_user','0'))
        
        result=""
        if notification_type=='result':
            if LotteryTicket.objects.filter(id=for_ticket).exists():
                result=notify_draw_result_for_ticket(for_ticket)
            elif LotteryDraw.objects.filter(id=for_draw).exists():
                result=notify_draw_result_all(for_draw)
        else:
            try:
                #if YooLottoUser.objects.filter(id=for_user).exists():
                user = YooLottoUser.objects.get(id=for_user)
                draw=LotteryDraw.objects.get(id=for_draw)
                component_name = draw.component.name
                draw_value = "{:,}".format(draw.jackpot)
                game_type = EnumerationManager.game_reverse(draw.component.parent.pk)
                result=notify_frenzy_for_user_without_ticket(component_name, draw_value, game_type, user.id,draw.id)
            except Exception,e:
                import traceback
                result=str(traceback.format_exc())
        
        
        return HttpResponse("TYPE (result / frenzy) = "+str(notification_type)+"<br>" \
                            "for_draw = "+str(for_draw if for_draw else "NOT FOUND")+"<br>" \
                            "for_ticket = "+str(for_ticket if for_ticket else "NOT FOUND")+"<br>" \
                            "for_user = "+str(for_user if for_user else "NOT FOUND")+"<br>" \
                             "----------------------------------<br>"+\
                             result+"<br><br>"\
                             
                            )
        
        
    
    def post(self,request):
        return HttpResponse(" IDIOT ")
        
class FindTickets(View):
    def get(self,request):
        of_draw=request.GET.get('of_draw','')
        total=0
        tickets=None
        last=None
        
        if not of_draw:
            return HttpResponse("!!  of_draw not provided  !!")
        if LotteryDraw.objects.filter(id=of_draw).exists():
            if LotteryTicket.objects.filter(draw__id=int(of_draw)).exists():
                total=LotteryTicket.objects.filter(draw__id=int(of_draw)).count()
                if LotteryTicket.objects.filter(draw__id=int(of_draw)).exists():
                    tickets=LotteryTicket.objects.filter(draw__id=int(of_draw)).values('pk')
                    last=LotteryTicket.objects.filter(draw__id=int(of_draw)).order_by('-id')[0].id
        else:
            return HttpResponse("!!  INVALID DRAW ID  !!")
        
        
        result="Total = "+str(total)+"<br>" \
               "Tickets Found ="+str(tickets)+"<br>" \
               "Last ticket ="+str(last)
        return HttpResponse(result)
         
         
     
    def post(self,request):
        return HttpResponse(" IDIOT ")
        
                            
class FindUser(View):
    def get(self,request):
        of_ticket=request.GET.get('of_ticket','')
        
        user_id=None
        
        if not of_ticket:
            return HttpResponse("!!  of_ticket not provided  !!")
        
        if LotteryTicket.objects.filter(id=of_ticket).exists():
            user_id=LotteryTicket.objects.get(id=of_ticket).user.id
        else:
            return HttpResponse("!!  INVALID TICKET ID  !!")
        
        
        result=" User ID = "+str(user_id)+"<br>" \
               " For Ticket ="+str(of_ticket)+"<br>" \
               
        return HttpResponse(result)
         
         
     
    def post(self,request):
        return HttpResponse(" IDIOT ")
    
class FindLastUser(View):
    def get(self,request):
        #of_ticket=request.GET.get('of_ticket','')
        
        HELP_TEXT=" >>> Ensure no one is hitting ,this time ! <<< "
        total_user="NOT FOUND"
        user_id="NOT FOUND"
        try:
            user_id=YooLottoUser.objects.order_by('-id')[0].id
            total_user=YooLottoUser.objects.count()
        except Exception as e:
            HELP_TEXT=str(e)
        
        result=" <br> "+str(HELP_TEXT)+" <br>" \
               " Total Users = "+str(total_user)+"<br>" \
               " Last User ID = "+str(user_id)+"<br>" \
               
        return HttpResponse(result)
         
         
     
    def post(self,request):
        return HttpResponse(" IDIOT ")
                            
def notify_draw_result_all(draw_id):
    from yoolotto.lottery.models import LotteryTicket
    result=''
    for _id in set([x["pk"] for x in LotteryTicket.objects.filter(
        draw_id=int(draw_id), submissions__checked=False).values("pk")]):
        result=result+"\n\n"+notify_draw_result_for_ticket(_id)
    return result

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
            print apn.send()
            log += "\nAPN Outbound: %s" % device.device_token
        elif device.is_android():
            gcm = GCMSender(to=[device.device_token], data={"text": text,
                                                            "code": "draw", "drawId": ticket.draw.pk,
                                                            "state":ticket.division.remote_id,
                                                            "ticket_id":ticket.id
                                                            })
            print gcm.send()
            log += "\nGCM Outbound: %s" % device.device_token
    
    #ticket.notified = True
    ticket.save()
    
    return log

def notify_frenzy_for_user_without_ticket(component_name, draw_value, game_type,user_id,draw_id):
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
    
   
# draw_id=-1
# print LotteryTicket.objects.filter(draw_id=int(draw_id)).count()
# print LotteryTicket.objects.filter(draw_id=int(draw_id)).last().id#values("pk")

#print notify_draw_result_for_ticket(33369)



# 
# def notify_frenzy_for_user(component_name, draw_value, game_type,ticket,draw_id):
#     from yoolotto.user.models import YooLottoUser
#     from yoolotto.communication.apn import APNSender
#     from yoolotto.communication.gcm_sender import GCMSender
#     
#     log = ""
#     text = "%s Jackpot is %s!" % (component_name, draw_value)
#     log += "\n" + text
#     
#     for device in ticket.user.devices.all():
#         if not device.device_token:
#             log += "\nSkipped Device, No Token"
#             continue
#         
#         if device.is_ios():
#             apn = APNSender(device.device_token, text=text, 
#                 custom={"code": "frenzy", "gameType": game_type,"state":state_codes[ticket.division.remote_id]})
#             apn.send()
#             log += "\nAPN Outbound: %s" % device.device_token
#         elif device.is_android():
#             gcm = GCMSender(to=[device.device_token], data={"text": text,
#                 "code": "frenzy", "gameType": game_type,"state":ticket.division.remote_id,"draw_id":draw_id
#             })
#             gcm.send()
#             log += "\nGCM Outbound: %s" % device.device_token
#         
#     return log

