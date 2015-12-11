import datetime

from apns import APNs, Payload, PayloadTooLargeError
from celery.task import PeriodicTask, task
from django.conf import settings

from yoolotto.util.async import report_errors
from yoolotto.user.models import Device

class UpdateAPNSFeeedback(PeriodicTask):
    run_every = datetime.timedelta(minutes=15)

    @report_errors
    def run(self, **kwargs):
        apn = APNs(use_sandbox=settings.NOTIFICATIONS_SANDBOX, 
            cert_file=settings.NOTIFICATIONS_APNS_CERT_PATH, 
            key_file=settings.NOTIFICATIONS_APNS_KEY_PATH)
                
        for (token_hex, fail_time) in apn.feedback_server.items():
            for device in Device.objects.filter(device_token=token_hex):
                device.device_token = None
                device.save()