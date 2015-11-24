from django.contrib import admin
from yoolotto.coin.models import *

class EmailCoinsInfoAdmin(admin.ModelAdmin):
        list_display = ['email','coins','dollar_amount']

admin.site.register(EmailCoins,EmailCoinsInfoAdmin)
