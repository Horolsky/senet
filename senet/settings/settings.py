from senet.utils import singleton
from os.path import isfile#, isdir
#from os import mkdir
from json import load, dumps
from shutil import copyfile 
class settings(metaclass=singleton):
    """
    settings singletone manager
    operates on ./senet_settings.json
    if file not exict, creates default
    """
    def __init__(self):
        self._fpath = "senet_settings.json"
        if not isfile(self._fpath):
            self.reset()
    def reset(self):
        """
        reset settings to default conditions
        """
        template = "senet/settings/settings.json" 
        copyfile(template, self._fpath)
    def __getall(self):
        f = open(self._fpath, "r") 
        settings = load(f) 
        f.close()
        return settings
    def get(self, path):
        """
        pass a setting path in form "group/setting"
        or "all" to get all settings as 2d dict
        """
        if path == "all":
            return self.__getall()
        if type(path) is not str or "/" not in path:
            return None
        f = open(self._fpath, "r") 
        settings = load(f) 
        f.close()
        group, st = path.split("/")
        try:
            return settings[group][st]["value"]
        except:
            return None
    def set(self, path, value):
        """
        pass a setting path in form "group/setting"
        and a new value
        """
        if type(path) is not str or "/" not in path:
            return False
        
        f = open(self._fpath, "r") 
        settings = load(f) 
        f.close()
        group, st = path.split("/")
        oldval = None
        try:
            oldval = settings[group][st]["value"] 
        
            if type(oldval) is list and type(value) is not list:
                value = value.replace(" ", "").replace(",", "")
                value = [int(c) for c in value]
                options = settings[group][st]["options"]
                if len(set(value) - set(options)) != 0:
                    return False
            else:
                value = type(oldval)(value)
                if value not in settings[group][st]["options"]:
                    return False

            
            settings[group][st]["value"] = value
            settings = dumps(settings)
            f = open(self._fpath, "w")
            f.write(str(settings)) 
            f.close()
            return True
        except:
            return False
        
        