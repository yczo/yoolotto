import json
import urllib2

class SevenElevenHandler(object):
    NAME = "7Eleven"
    
    @staticmethod
    def retrieve(latitude, longitude, range=5, size=10):
        data = {
            "SourceLatitude": latitude,
            "SourceLongitude": longitude,
            "SearchRangeMiles": "%.2f" % range,
            "PageSize": size,
            "PageNumber": 0,
            "Filters": [],
        }
        
        _data = json.dumps(data)
        req = urllib2.Request("https://www.7-eleven.com/api/location/SearchStores")
        req.add_header("Content-Type", "application/json")
        req.add_header("Content-Length", len(_data))
        req.add_data(_data)
        try:
            response = urllib2.urlopen(req)
        except urllib2.HTTPError:
            return []
        
        raw = json.loads(response.read())
        result = []
        for item in raw:
            _result = {
                "name": "7-Eleven",
                "code": SevenElevenHandler.NAME,
                "address": item.get("Address1", None),
                "address_2": item.get("Address2", None),
                "city": item.get("City", None),
                "state": item.get("State", None),
                "postal_code": item.get("Zip", None),
                "phone": item.get("Phone", None),
                "latitude": item.get("Latitude", None),
                "longitude": item.get("Longitude", None),
                "distance": item.get("DistanceFromSearch", None)
            }
            
            result.append(_result)        
        return result

        