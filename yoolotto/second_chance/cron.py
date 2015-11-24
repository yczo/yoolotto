import cronjobs
import time
from django.db import transaction
from yoolotto.settings import AFTER_LOGON_OX, email, password, domain, realm, consumer_key, consumer_secret
import urllib2
import json
from yoolotto.second_chance.models import AdInventory as InventoryModel, Advertisor as AdvertisorModel

@cronjobs.register
@transaction.commit_on_success  
def populateAds():
    import ox3apiclient

    ox = ox3apiclient.Client(
        email=email,
        password=password,
        domain=domain,
        realm=realm,
        consumer_key=consumer_key,
        consumer_secret=consumer_secret,
        api_path='/ox/4.0'
        )

    AFTER_LOGON_OX=ox.logon(email, password)

    ad_ids = []
    while True:
        adunits =  AFTER_LOGON_OX.get('http://ox-ui.yoolotto.com/ox/4.0/adunit?type_full=adunit.native&limit=0')['objects']
        for adunit in adunits:
            vendor = None
            vendor_image = None
            coins = 0
            timer = False
            time_object = 0
            ad_type = None
            inventory = 0
            try:
                url = 'http://ox-d.yoolotto.com/ma/1.0/arj?auid='+adunit['id']
                data = json.loads(urllib2.urlopen(url).read())
                ad_data = data['ads']['ad']
                if ad_data :
                    media_info = json.loads(ad_data[0]['creative'][0]['media'])
                    
                    if media_info.has_key('vendor'):
                        vendor = media_info['vendor']
                        
                    if media_info.has_key('vendor_image'):
                        vendor_image = media_info['vendor_image']
                        
                    if media_info.has_key('coins'):
                        coins = media_info['coins']
                        
                    if media_info.has_key('timer'):
                        timer = media_info['timer']
                        
                    if media_info.has_key('time'):
                        time_object = media_info['time']
                        
                    if media_info.has_key('type'):
                        ad_type = media_info['type']
                        
                    if media_info.has_key('inventory'):
                        inventory = media_info['inventory']
                        
                    if vendor:
                        try:
                            vendor_record= AdvertisorModel.objects.get(name=vendor)
                            vendor_record.image = vendor_image
                        except:
                            vendor_record = AdvertisorModel(name=vendor,image=vendor_image)
                        vendor_record.save()
                        try:
                            InventoryRecord = InventoryModel.objects.get(id=adunit['id'])
                            InventoryRecord.name = adunit['name']
                            InventoryRecord.inventory = inventory
                            InventoryRecord.type = ad_type
                            InventoryRecord.status = adunit['status']
                            InventoryRecord.timer = timer
                            InventoryRecord.time = time_object
                            InventoryRecord.account = vendor_record
                            InventoryRecord.ad_id = ad_data[0]['adid']
                            InventoryRecord.coins = coins
                        except:
                            InventoryRecord = InventoryModel(id=adunit['id'],name=adunit['name'],inventory=inventory,type=ad_type,status=adunit['status'],timer=timer,time=time_object,account=vendor_record,ad_id=ad_data[0]['adid'],coins=coins)
                        InventoryRecord.save()
                        ad_ids.append(adunit['id'])
            except:
                    pass
        InventoryModel.objects.exclude(id__in=ad_ids).update(status='Inactive')
        transaction.commit()
        time.sleep(300)

@cronjobs.register
@transaction.commit_on_success                                           
def reset_coins():
        import datetime
        now = datetime.datetime.now()
        from datetime import timedelta
        coins_record = FantasyTicket.objects.filter(added_at__lte = reset_date)
        reset_date = datetime.datetime.now() + datetime.timedelta(-90)
        