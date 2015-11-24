from django.shortcuts import render_to_response
from yoolotto.yoolotto_debug.invalid_powerball_result import remove_invalid_powerball_results
from django.contrib.auth.decorators import login_required
from yoolotto.yoolotto_debug.tasks import bulk_notification

@login_required(login_url="/admin")
def debug_home(req):
    debug_dict={
                   'D1':"This will find and correct all invalid Powerball draw's result of 7 Numbers."
               }
    
    if req.POST.get("debug_D1",False):
        current_type='D1'
        current_header=debug_dict[current_type]
        data=remove_invalid_powerball_results()
        note="If you got record please keep it for your purpose, debugging next time you may got different results !"
        return render_to_response("yoolotto_debug/home.html",
                                  {'current_type':current_type,
                                   'current_header':current_header,
                                   'all_debug':debug_dict,
                                   'data':data,
                                   'note':note
                                  }
                                  )
    return render_to_response("yoolotto_debug/home.html" ,{'all_debug':debug_dict})


@login_required(login_url="/admin")
def data_entry_images(req):
  from yoolotto.lottery.models import LotteryTicketAvailable
  unique_tx_images = []
  unique_ca_images = []
  texas_tickets = []
  cailifornia_tickets = []
  tx_tickets = LotteryTicketAvailable.objects.filter(ticket__division_id=1).exclude(image_first="").order_by('-id').values_list('play_id','image_first','defected').distinct()[0:250]
  ca_tickets = LotteryTicketAvailable.objects.filter(ticket__division_id=2).exclude(image_first="").order_by('-id').values_list('play_id','image_first','defected').distinct()[0:250]

  for item in tx_tickets:
    if item[1] not in unique_tx_images:
        unique_tx_images.append(item[1])
        texas_tickets.append(item)

  for item in ca_tickets:
    if item[1] not in unique_ca_images:
        unique_ca_images.append(item[1])
        cailifornia_tickets.append(item)

  return render_to_response("yoolotto_debug/dte_images.html" ,{'tx_tickets':texas_tickets,'ca_tickets':cailifornia_tickets})

@login_required(login_url="/admin")
def send_bulk_notification(req):
    if req.POST.get("send_notification",False):
        data={}
        device_type=req.POST.get("device_type","all")
        message=req.POST.get("message","").strip()
        
        if not message:
            data.update({'has_messages':['please enter notification message.']})
        else:
            try:
                #test_task.apply_async(countdown=0,kwargs={})
                bulk_notification.apply_async(countdown=0,kwargs={"of_type":device_type,"for_message":message,'data':{}})
                data.update({'has_pressed':True,'has_messages':['Bulk notification has started successfully, <a href="/flower">click here to see status.</a>']}) 
            except Exception,e:
                data.update({'has_pressed':True,'has_messages':[str(e)]})
                                                            
        
        return render_to_response("yoolotto_debug/notification.html",
                                  data
                                  )
    return render_to_response("yoolotto_debug/notification.html")


