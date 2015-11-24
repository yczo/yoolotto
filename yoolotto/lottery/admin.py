from django.contrib import admin
from yoolotto.lottery.models import *

class LotteryGameAdmin(admin.ModelAdmin):
	list_display = ['name', 'code', 'active']


class LotteryCountryDivisionAdmin(admin.ModelAdmin):
	list_display = ['remote_id', 'remote_country']

class LotteryGameComponentAdmin(admin.ModelAdmin):
	list_display = ['name', 'parent','format', 'identifier']
	filter_horizontal = ['division']

class LotteryTicketAvailableAdmin(admin.ModelAdmin):
        list_display = ['ticket', 'play', 'available', 'json', 'image_first', 'valid_image_name', 'rejected', 'reason' ]


class LotteryDrawAdmin(admin.ModelAdmin):
	list_display = ['id','jackpot','division', 'result','component','_parent', 'date']
	search_fields = ['component__name',]
	list_filter = ['component__format', 'component__division__remote_id']

	def _parent(self, obj):
		return obj.component.name

class LotteryDrawFrenzyAdmin(admin.ModelAdmin):
	list_display = ['draw', 'added_at']

class LotteryTicketAdmin(admin.ModelAdmin):
	list_display = ['id']

class LotteryTicketSubmissionAdmin(admin.ModelAdmin):
	list_display = ['submission', 'ticket', 'added_at']

class LotteryTicketPlayAdmin(admin.ModelAdmin):
	list_display = ['play', 'ticket', 'submission', 'winnings']

class UserTestimonialsAdmin(admin.ModelAdmin):
	list_display = ['name', 'content', 'profile_pic']

class RulesAndInfoAdmin(admin.ModelAdmin):
	list_display = ['heading','content']

class FantasyHelpInfoAdmin(admin.ModelAdmin):
        list_display = ['question','answer']

class FantasyGameInfoAdmin(admin.ModelAdmin):
        list_display = ['gameinfo']

admin.site.register(LotteryGame, LotteryGameAdmin)
admin.site.register(LotteryCountryDivision, LotteryCountryDivisionAdmin)
admin.site.register(LotteryGameComponent, LotteryGameComponentAdmin)
admin.site.register(LotteryDraw, LotteryDrawAdmin)
admin.site.register(LotteryDrawFrenzy, LotteryDrawFrenzyAdmin)
admin.site.register(LotteryTicket, LotteryTicketAdmin)
admin.site.register(LotteryTicketSubmission, LotteryTicketSubmissionAdmin)
admin.site.register(LotteryTicketAvailable,LotteryTicketAvailableAdmin)
admin.site.register(UserTestimonials,UserTestimonialsAdmin)
admin.site.register(Fantasy_Help_Info,FantasyHelpInfoAdmin)
admin.site.register(Fantasy_Game_Info,FantasyGameInfoAdmin)
