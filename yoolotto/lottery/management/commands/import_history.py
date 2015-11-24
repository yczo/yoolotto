from django.core.management.base import BaseCommand, CommandError

from yoolotto.lottery.feed.history import LotteryTicketHistoryImporter

class Command(BaseCommand):
    help = 'Manual trigger for performing history import  '

    def handle(self, *args, **options):
        importer = LotteryTicketHistoryImporter()
        importer.run()        
    