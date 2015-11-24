from django.core.management.base import BaseCommand, CommandError

from yoolotto.lottery.models import LotteryTicket

class Command(BaseCommand):
    help = '''
    Recalculate the winnings, coupons, and coins for every Ticket. This is a 
    VERY slow operation intended for testing only.
    
    '''
    def handle(self, *args, **options):
        for ticket in LotteryTicket.objects.all():
            ticket.update(full=True)