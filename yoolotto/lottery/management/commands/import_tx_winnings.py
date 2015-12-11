from django.core.management.base import BaseCommand, CommandError
from yoolotto.lottery.feed.data import LotteryWinningTXDataProvider
from yoolotto.lottery.feed.winning_tx import LotteryWinningTXImporter

class Command(BaseCommand):
    help = 'Manual trigger for performing feed updates  '

    def handle(self, *args, **options):
        winning_tx = LotteryWinningTXImporter(provider=LotteryWinningTXDataProvider.remote)
        winning_tx.run() 