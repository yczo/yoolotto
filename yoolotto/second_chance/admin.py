from django.contrib import admin
from yoolotto.second_chance.models import *


class FaqAdmin(admin.ModelAdmin):
    """
    displaying the faq page
    """
    
    list_display = ['id','question','answer']
    
admin.site.register(FAQ, FaqAdmin)

class AdInventoryAdmin(admin.ModelAdmin):
    """
    displaying the faq page
    """
    
    list_display = ['id','name','inventory','type']
    
admin.site.register(AdInventory, AdInventoryAdmin)

class FantasyFaqAdmin(admin.ModelAdmin):
    list_display = ['id','question','answer']

class DwollaFaqAdmin(admin.ModelAdmin):
    list_display = ['id','question','answer']


admin.site.register(FantasyFAQ, FantasyFaqAdmin)
admin.site.register(DwollaFAQ, DwollaFaqAdmin)
