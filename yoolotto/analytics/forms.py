from django import forms
from django.forms.widgets import *
from yoolotto.lottery.models import LotteryGame
from yoolotto.lottery.models import LotteryGameComponent


CHOICES = (
	('------','------'),
	('Powerball','Powerball'),
	('MEGA Millions','MEGA Millions'),
	('Pick Three','Pick Three'),
	('All or Nothing','All or Nothing'),
	('Cash Five','Cash Five'),
	('Daily Four','Daily Four'),
	('Two Step','Two Step'),
	('Lotto Texas','Lotto Texas'),
	)

class AnalyticForm(forms.Form):
	game = forms.CharField(label="Game",
							max_length=80,
							widget =Select(choices=CHOICES),
							required=True,
							)