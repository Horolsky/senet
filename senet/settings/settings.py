from senet.utils import singleton
from os.path import isfile#, isdir
#from os import mkdir
from json import load, dumps
from shutil import copyfile 

class settings(metaclass=singleton):
    """
    singletone settings manager
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
        settings = self.__getall()
        if path == "all":
            return settings
        if type(path) is not str or "/" not in path:
            return None
        group, st = path.split("/")
        try:
            value = settings[group][st]["value"] 
            return value
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
        
            if type(oldval) is list and type(value) is list:
                value = [int(c) for c in value]
                options = settings[group][st]["options"]
                if len(set(value) - set(options)) != 0:
                    return False
                if len(value) not in settings[group][st]["lrange"]:
                    return False
            else:
                if str(value).lower() == "true":
                    value = True
                elif str(value).lower() == "false":
                    value = False
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

SETTINGS = settings()
"""
instance of a settings managment singleton class
"""