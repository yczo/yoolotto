from django.core.management.base import BaseCommand, CommandError

from yoolotto.coin.models import CoinTicketTransaction, CoinSubmissionTransaction
from yoolotto.lottery.models import LotteryTicket
from yoolotto.user.models import YooLottoUser


class Command(BaseCommand):
    def handle(self, *args, **options):
        
        for coin_ticket in CoinTicketTransaction.objects.all():
            transaction = coin_ticket.transaction
            
            coin_ticket.delete()
            transaction.delete()
            
        for coin_submission in CoinSubmissionTransaction.objects.all():
            transaction = coin_submission.transaction
            
            coin_submission.delete()
            transaction.delete()
        
        for ticket in LotteryTicket.objects.all().order_by("user__id"):
            ticket.update_coins(save=True)