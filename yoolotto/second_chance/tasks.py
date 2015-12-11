from yoolotto.second_chance.models import DeviceLoginDetails
from yoolotto.coin.models import DeviceCoins, EmailCoins
from celery.task import task
from celery.task.schedules import crontab
from celery.decorators import periodic_task
import datetime

@periodic_task(run_every=crontab(hour='5', minute='0', day_of_week="*"))
def removecoins():
	now = datetime.datetime.now()
	reqdate = now - datetime.timedelta(30)
	email_info = EmailCoins.objects.filter(reset_date__lte = reqdate)
	for emial in email_info:
	    emial.coins = 0
	    emial.reset_date = now
	    emial.save()
	DeviceCoins.objects.filter(reset_date__lte = reqdate).update(coins = 0, reset_date = now)
