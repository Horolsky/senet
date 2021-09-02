from os import mkdir
from os.path import isdir, isfile
from datetime import datetime
class Report:
    def __init__(self, prefix, ext="txt", path="logs", msg=0, hold=True):
        """
        logging game events to file
        """
        
        if type(prefix) is not str:
            raise TypeError("invalid file prefix arg")
        if type(ext) is not str:
            raise TypeError("invalid file extension arg")
        if type(path) is not str:
            raise TypeError("invalid path arg")
            
        #dir check
        dirs = path.split('/')
        for i in range(len(dirs)):
            subdir = '/'.join(dirs[:i+1])
            if not isdir(subdir):
                mkdir(subdir)
        self._hold = hold
        self._fname = f"{path}/{prefix}.{ext}"
        if isfile(self._fname):
            self._file = open(self._fname, "a")
        else:
            self._file = open(self._fname, "a")
            #init msg
            if type(msg) is str:
                self._file.write(msg)
            if not self._hold:
                    self._file.close()

    def __del__(self):
        self._file.close()

    def write(self, msg):
        if type(msg) is str:
            if not self._hold:
                self._file = open(self._fname, "a")
                self._file.write(msg)
                self._file.close()
            else:
                self._file.write(msg)
            return True
        return False
        
    def close(self, msg=0):
        if type(msg) is str:
            self._file.write(msg)
        self._file.close()