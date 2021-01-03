from os import mkdir
from os.path import isdir
from datetime import datetime
class report:
    def __init__(self, prefix, ext="txt", directory="logs", msg=0):
        timestamp = str(datetime.now()).replace(":", "").replace("-", "").replace(" ", "-")[:15]
        if type(prefix) is not str:
            raise TypeError("invalid file prefix arg")
        if type(ext) is not str:
            raise TypeError("invalid file extension arg")
        if type(directory) is not str:
            raise TypeError("invalid directory arg")
            
        #dir check
        if not isdir(directory):
            mkdir(directory)
        
        self._fname = f"{directory}/{prefix}-{timestamp}.{ext}"
        self._file = open(self._fname, "a")
        #init msg
        if type(msg) is str:
            self._file.write(msg)

    def __del__(self):
        self._file.close()

    def write(self, msg):
        if type(msg) is str:
            self._file.write(msg)
            return True
        return False
        
    def close(self, msg=0):
        self._file.close()