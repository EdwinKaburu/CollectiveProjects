"""
@author Edwin Kaburu
@date 12/3/2022
@see "Seattle University, CPSC 5520, Fall 2022" - Distributed Systems
@file fileabstraction.py
Lab6 Extra Credit

README:
    * Low-level File Handling Of Reading, Writing Files Bytes Information
        * Not All Feature Required From Client Have Been Implemented
    * Keep Track of Directory
    * Virtual Mapper Dictionary is Kept.
        * Key -> SHA-1(File Name)
        * Value -> FileName
    * Network To Get And Send Response Synchronously abstracted here. Shared by both client and main parties

"""

import hashlib
import pickle
from pathlib import *
from socket import *


def node_get_response_sync(socket_t, set_timer=False):
    """
    node_get_response_sync() Get Response from Socket
    :param socket_t: Socket Instance
    :param set_timer: Boolean Timer Condition
    :return: Data from Socket
    """
    try:
        server_host_response = b''
        counter = 0
        while True:
            if set_timer:
                # Set a 5 seconds timeout
                socket_t.settimeout(5)
            # Save to response message to 4096 bytes buffer
            counter += 4096
            try:
                pkt_data = socket_t.recv(4096)
            except TimeoutError:
                break
            else:
                if pkt_data == b'':
                    break
                else:
                    server_host_response += pkt_data

    except Exception as error:
        return None
    else:
        # No Errors Raised
        if server_host_response == b'':
            return None
        # return deserialized response
        return pickle.loads(server_host_response)


def node_send_network_message(message, connection_tuple: (), set_timer=False, skip_response=False):
    """
    node_send_network_message() Send Message To Certain Peer End-Point
    :param message: Message
    :param connection_tuple: End-Point Tuple
    :param set_timer: Boolean Timer Condition
    :param skip_response: Boolean Await Response Condition
    :return: Response Object
    """
    # Act as a Client, asking An Existing Node To Find Successor
    client_socket = socket(AF_INET, SOCK_STREAM)

    host_response = None
    try:
        # Connect To Existing-Node-Host
        client_socket.connect(connection_tuple)
    except Exception as error_msg:
        # Print Exception message
        print(error_msg)
    else:
        # Send Node's Identifier To Existing-Node-Host
        client_socket.send(pickle.dumps(message))

        if not skip_response:
            # Get Response from Existing-Node-Host
            host_response = node_get_response_sync(client_socket, set_timer)

    # Close Client Connection Socket
    client_socket.close()
    # Return Result
    return host_response


"""
@class FileAbstraction : Low Level File Abstraction
"""


class FileAbstraction:
    # Support File Extensions
    supported_extensions = ['.png', '.txt', '.csv', '.jpg', 'jpeg', '.pdf']
    # Node Cache Directory
    node_net_cache_dir = None
    # Node Cached Files
    node_net_cached_files_dict = dict()
    # File Buffer Data
    file_buffer_data = None

    def __init__(self, file_path="", is_network_node=False):
        """
        __init__() Default Constructor for Class
        :param file_path: String File Path
        :param is_network_node: Boolean Condition If Network Or Not
        """
        if is_network_node:
            # Create Directory Folder, if it does not exist
            self.node_net_cache_dir = Path("cache/")
            self.node_net_cache_dir.mkdir(parents=True, exist_ok=True)
            print(self.node_net_cache_dir.suffix)

        # Write File To Local Cache
        self.fa_read_write_file(file_path)

    def fa_read_write_file(self, file_path):
        """
        fa_read_write_file() Read File from Disk and Write To Cache Directory
        :param file_path: String FIle Path
        :return: None
        """
        if file_path != "":
            # Read File From Disk
            self.fa_read_file(file_path)
            # Write Bytes To File
            exec_status = self.fa_write_file(file_path)
            if not exec_status:
                print(f"Error Occurred: {file_path}\n"
                      f"---> Either File Extension Not Supported Or File Does Not Exists Or File Duplicate Found")

    def fa_write_bytes(self, file_string, data_bytes):
        """
        fa_write_bytes() Write Bytes of Data to Cache Directory Under File String Name
        :param file_string: String File String
        :param data_bytes: Bytes of Data
        :return: Boolean Condition
        """
        # Update Bytes Buffer
        self.fa_update_file_buffer_bytes(data_bytes)
        # Write the Bytes under File String
        return self.fa_write_file(file_string)

    def fa_read_bytes(self, file_string):
        """
        fa_read_bytes() Read Bytes of File Stored in Cache Directory
        :param file_string: String File String
        :return: Bytes of Data
        """
        # Default Bytes Offered
        bytes_return = b''

        if file_string != '':
            # Get File Name
            file_name = Path(file_string).name
            # Get Hashed Key
            hashed_file_key = self.fa_get_file_name_hash(file_name)

            if hashed_file_key in self.node_net_cached_files_dict:
                # Local Cache File Path
                local_file_path = './cache/' + file_name
                # Read file cached
                cached_status = self.fa_read_file(local_file_path)
                if cached_status:
                    bytes_return = self.file_buffer_data

        return bytes_return

    def fa_is_extension_supported(self, extension):
        """
        fa_is_extension_supported() Validate if File Extension is Support
        :param extension: String File Extension
        :return: Boolean Condition
        """
        for fa_extension in self.supported_extensions:
            # Validate
            if extension.lower() == fa_extension:
                # Supported File Extension
                return True
        # Not Support File Extension
        return False

    def fa_is_unique_file(self, file_string: str, is_file_path=False):
        """
        fa_is_unique_file() Validate if file-string is unique from existing file-strings of cache directory
        :param file_string: String File String
        :param is_file_path: Boolean Condition
        :return: Boolean Condition
        """
        # Default. Parameter not file path, save file name
        file_name = file_string
        # Parameter is a File Path
        if is_file_path:
            # Get File Name from File Path
            file_name = Path(file_string).name

        # Hash File Name
        hashed_file_key = self.fa_get_file_name_hash(file_name)

        # Validate Duplicate
        if hashed_file_key in self.node_net_cached_files_dict:
            # Copy Already Exists
            return False
        # Unique File
        return True

    def fa_write_file(self, file_path: str):
        """
        fa_write_file() Write Files Bytes of Data to cache directory under file path name
        :param file_path: String File
        :return: Boolean Condition
        """
        # Update Dictionary Before Writing
        self.fa_update_cached_file_dict()

        # Validate File Path
        if file_path != '':
            # Get File Name
            file_name = Path(file_path).name
            # Local Cache File Path
            local_file_path = self.node_net_cache_dir / file_name

            # Buffer Not Empty And File Name is Unique
            if self.file_buffer_data is not None and self.fa_is_unique_file(file_name):
                try:
                    # Save Bytes Data To New Location
                    with local_file_path.open('wb') as file_wb:
                        # Write The Bytes Data
                        file_wb.writelines(self.file_buffer_data)
                except Exception as file_error:
                    return False
                else:
                    # Update Dictionary After Writing
                    self.fa_update_cached_file_dict()
                    # Success
                    return True
        # Exception
        return False

    def fa_read_file(self, file_path: str):
        """
        fa_read_file() Read file from file path and save the file buffer bytes
        :param file_path: String File Path
        :return: Boolean Condition
        """
        # Validate File Path
        if file_path != '':
            # Get File Extension
            file_extension = Path(file_path).suffix
            # Validate Extension Supported
            if self.fa_is_extension_supported(file_extension):
                try:
                    with open(file_path, 'rb') as file_rb:
                        # Get File Bytes
                        self.file_buffer_data = file_rb.readlines()
                except Exception as file_error:
                    # Exception Raised
                    return False
                else:
                    # Success
                    return True
        # Exception
        return False

    def fa_update_file_buffer_bytes(self, new_bytes):
        """
        fa_update_file_buffer_bytes() Overwrite Existing Bytes of File buffer
        :param new_bytes: New Bytes of Data
        :return: None
        """
        self.file_buffer_data = new_bytes

    def fa_update_cached_file_dict(self):
        """
        fa_update_cached_file_dict() Update Mapper Dictionary
        :return: None
        """
        if self.node_net_cache_dir is not None:
            for saved_file in self.node_net_cache_dir.iterdir():
                # Get Hash
                hash_file_key = self.fa_get_file_name_hash(saved_file.name)
                # Save Files
                self.node_net_cached_files_dict.update({hash_file_key: saved_file.name})

    @staticmethod
    def fa_get_file_name_hash(file_name: str):
        """
        fa_get_file_name_hash() Hash a File Name
        :param file_name:  String File Name
        :return:Hex Hashed File Name
        """
        # Hash FileName To SHA-1
        hashed_file_key = hashlib.sha1(file_name.encode()).hexdigest()
        # return hashed value
        return hashed_file_key

# if __name__ == '__main__':
# absolute_file_path = r'C:\Users\Edwin\Downloads\Picture1.png'
# Node_File = FileAbstraction(absolute_file_path, True)

# Attempt To Add Duplicate File
# Node_File.fa_read_write_file(absolute_file_path)

# Add Different File
# absolute_file_path1 = r'C:\Users\Edwin\Downloads\1-Research and innovation.pdf'
# Node_File.fa_read_write_file(absolute_file_path1)

# get_file_bytes

# bytes_print = Node_File.fa_read_bytes(absolute_file_path)

# print("End Of Program")
