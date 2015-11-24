import os
import json
import time
import datetime

from django.core.management.base import BaseCommand, CommandError
from django.conf import settings

from yoolotto.user.models import Device
from yoolotto.communication.apn import APNSender
from yoolotto.communication.gcm_sender import GCMSender

class Command(BaseCommand):
    def handle(self, *args, **options):
        file = os.path.join(settings.LOG_BASE_PATH, "mass_notification-%s.log" 
            % datetime.datetime.now().isoformat())
        fp = open(file, "w")
        
        batch = []
        batches = []
        
        query = Device.objects.filter(device_type="ANDROID", device_token__isnull=False)
        query = Device.objects.filter(user_id=2441)
        
        for device in query:
            if len(batch) >= 500:
                batches.append(batch)
                batch = []
            batch.append(device.device_token)
                
        if batch:
            batches.append(batch)
        
        data = {"text": "Fixes coming soon, so sorry!", "code": "frenzy"}
        
        fp.write("GCM Data\n")
        fp.write(json.dumps(data) + "\n\n")
        
        for batch in batches:
            fp.write("Sending GCM Batch to %s Recipients\n" % len(batch))
            fp.write(json.dumps(batch) + "\n\n")
            
            try:
                gcm = GCMSender(to=batch, data=data)
                gcm.send()
            except Exception as e:
                fp.write(str(e))
                raise
            
        query = Device.objects.filter(device_type="IPHONE", device_token__isnull=False)
        query = Device.objects.filter(user_id=20433)
        
        text = "Fixes coming soon, Sorry! Free food in Yoo Prizes"
        
        for device in query:
            fp.write("Sending one-off APN to %s" % device.device_token)
            
            try:
                apn = APNSender(device.device_token, text=text)
                apn.send()
            except Exception as e:
                fp.write(str(e))
                raise
            
            time.sleep(1)