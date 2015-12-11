from celery.task import task

                                            
@task
def bulk_notification(of_type,for_message,data={}):
    from yoolotto.user.models import Device
    log=""
    if of_type=='all':
        tokens=Device.objects.filter(device_token__isnull=False).exclude(device_token='').order_by('-id').values_list('device_token',flat=True).distinct()
    else:
        tokens=Device.objects.filter(device_token__isnull=False,device_type=of_type.upper()).exclude(device_token='').order_by('-id').values_list('device_token',flat=True).distinct()
    
    if not tokens:
        log="tokens not found"
        return log
    
    for token in tokens:
        try:
            device=Device.objects.filter(device_token=token)[0]
            notify_device.apply_async(countdown=0,kwargs={"device":device,"message":for_message,'data':data})
        except:
            pass

@task
def notify_device(device,message,data={}):
    from yoolotto.communication.apn import APNSender
    from yoolotto.communication.gcm_sender import GCMSender
    try:
        if device.is_ios():
            apn = APNSender(device.device_token,
                            text=message,
                            custom=data)
            apn.send()
        elif device.is_android():
            data={'code':'frenzy','draw_id':'12345'}
            
            data.update({'text':message})
            gcm = GCMSender(to=[device.device_token],data=data)
            gcm.send()
    except Exception,e:
        return str(e)


# @task
# def test_task():
#     import time
#     log=""
#     for x in range(1,5):
#         print "bulky =%s"%x
#         time.sleep(6)
#         log=log+","+str(x)
#      
#     log=log+"done"            
#     return log
#  



