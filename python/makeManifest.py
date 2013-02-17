# generate the SunSpec Manifest for the SMDX files
import dircache
import hashlib
import os.path
# Set the path where the smdx files are here
# linux path
path = "/home/jkn/sunspec/models/smdx"
# path = "~/sunspec/models/smdx"
# jkn MAC path
# path = "/Users/jkn/Documents/SunSpec/SunSpec Interoperability Specifications/SMDX"

# get a directory listing and iterate over the files
list = dircache.listdir(path)
md5Dict = {};
for file in list[:] :
    fullPath = os.path.join(path, file)
    # Exclude hidden files and the manifest file itself
    if file[0] != "." and file[0:8] != "manifest" :
        # read the file 
        f = open(fullPath, 'r')
        data = f.read()
        f.close()
        # compute the md5 hash
        hasher = hashlib.md5()
        hasher.update(data)
        md5 = hasher.hexdigest()
        # pop it into the dict
        md5Dict[file] =  md5
        print file, " : ", md5

# Output the Manifest file
manf = open(os.path.join(path, "manifest.xml"), "w")
manf.write("<manifest>\n")
for key in sorted(md5Dict.keys()) :
   md5 = md5Dict[key]
   manf.write("\t<file name=\""+key+"\" md5=\""+md5+"\"/>\n")
# end of manifest file
manf.write("</manifest>\n")
manf.close()

# md5 sum the manifest.xml file 
manf = open(os.path.join(path, "manifest.xml"), "r")
data = manf.read()
manf.close()
hasher = hashlib.md5()
hasher.update(data)
md5 = hasher.hexdigest()
manf = open(os.path.join(path, "manifest.xml.md5"), "w")
manf.write(md5+"\n")
manf.close()
