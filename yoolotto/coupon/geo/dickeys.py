from yoolotto.util.geo import haversine

class DickeysHandler(object):
    NAME = "Dickeys"

    @staticmethod
    def retrieve(latitude, longitude, range=5, size=10):
        distance = round(haversine(longitude, latitude, -96.802235, 32.794340)/1.609344, 2)
        if distance > 500:
            return []
        
        # 2324 Mckinney Ave, Dallas, TX
        return [{
            "name": DickeysHandler.NAME,
            "code": DickeysHandler.NAME,
            "address": "2324 Mckinney Ave",
            "address_2": None,
            "city": "Dallas",
            "state": "Texas",
            "postal_code": "75201",
            "phone": None,
            "latitude": 32.794340,
            "longitude": -96.802235,
            "distance": distance
        }]