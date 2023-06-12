"""
@author Edwin Kaburu
@date 12/3/2022
@see "Seattle University, CPSC 5520, Fall 2022" - Distributed Systems
@file client.py
Lab6 Extra Credit

README:
    * Prototype, The Delete Command is Not Implemented At This Time.
    * Download Command Works As Intended. You will receive bytes of the file, that you can save to local drive.
        * Note. FileAbstraction Have Not Implemented This Capability Yet.
        * The Bytes Can Be Viewed By Applying BreakPoints At Appropriate Sections.
    * Status Reports Are Displayed.
    * FAL - File Abstraction Layer
        * Responsible For Handling With Files And Their Bytes Of Information
        * Low Level
"""
import sys
import threading
from random import randrange
from fileabstraction import *

"""
@class ClientNodeProtocol : An Abstraction Of Client Saving File To Cloud
"""


class ClientNodeProtocol:
    # File Abstraction Layer Instance
    File_Abstract_Layer = FileAbstraction("", False)
    # Listening Socket
    listen_socket = socket()
    # Network Host
    net_host_endpoint = ()
    # Default ID Of Node
    identifier = -1
    # Client Thread
    client_user_thread = None
    # Listen Server Thread
    listen_server_thread = None
    # Boolean Condition
    continue_run = True
    # Default Cmd Rules
    cmd_rules_print = ""

    def __init__(self, ext_port, node_id):
        """
        __init__() Default Constructor of Class
        :param ext_port: Existing Node Port Number
        :param node_id: Random ID
        """

        # Initialize Variables
        self.net_host_endpoint = ('localhost', ext_port)
        # Client ID
        self.identifier = node_id
        # Set Up Listening Server
        self.cl_node_init_listen_socket()
        # Set Up Command Rules
        self.cmd_rules_print = f"\nAccepted Commands Names And Format Parameters: " \
                               f"\n--> All Command Name will be automatically converted to Capital" \
                               f"\n--> Upload <file_location_string>\n--> Delete <file_name>\n-->" \
                               f" Download <file_name>\n--> Ls\n--> Logout\n--> commands or help or cmd" \
                               f"\n---------------------------------------\n" \
                               f"Note 1: Ls, command similar to command prompt or " \
                               f"terminal command 'ls' and takes No Parameters" \
                               f"\nNote 2: <file_location_string> Absolute File Path To File:C:/Users/Disk1/cat.png" \
                               f"\nNote 3: <file_name> Just Name of File : cat.png\n Note 4: " \
                               f"To view instruction enter either, command or help or cmd" \
                               f"\n---------------------------------------\n" \
                               f"Support File Extensions:\n--> {self.File_Abstract_Layer.supported_extensions}\n"

        # Print Cmd Rules
        print(self.cmd_rules_print)

        # User Commands Handler
        self.client_user_thread = threading.Thread(target=self.cl_node_commands_main, name='ut1')
        # Start Thread
        self.client_user_thread.start()

        # Listen Server Handler
        # self.c_node_proto_listen_main()
        self.listen_server_thread = threading.Thread(target=self.cl_node_listen_main, name='ut2')
        self.listen_server_thread.start()

        # Join Thread
        self.client_user_thread.join()
        self.listen_server_thread.join()

        # End Of Program Print
        print(f"\nUser Command, And Main Thread Terminated")

    def cl_node_init_listen_socket(self):
        """
        cl_node_init_listen_socket() Setting Up Listening Server
        :return: None
        """
        # Create TCP/IP Listening Socket
        self.listen_socket = socket(AF_INET, SOCK_STREAM)
        # Bind the socket to Port and Name
        self.listen_socket.bind(('localhost', 0))
        # Listen To Connections
        self.listen_socket.listen(5)
        # Debugging Print
        print(f"{self.listen_socket.getsockname()}: Node's Listening Server Set Up Complete And Ready To Listening")

    def cl_node_commands_handler(self, user_input):
        """
        cl_node_commands_handler() User Commands Inputs And Events Handling
        :param user_input: String Command
        :return: Boolean Condition
        """

        # Split At 1st Space Occurrence
        input_split_list = user_input.split(" ", 1)
        # Get Command From String
        command_fetch = input_split_list[0]

        if command_fetch == 'LOGOUT':
            # Exit Program
            self.continue_run = False
            return True

        if command_fetch == 'LS':
            # Display All Files In Directory
            # Set Up Network Message
            net_message = (command_fetch, (self.listen_socket.getsockname(), ''))
            # Send Message To Server And Skip Response
            node_send_network_message(net_message, self.net_host_endpoint, False, True)
            return True

        if command_fetch == 'UPLOAD' and len(input_split_list) > 1:
            # Get File String
            file_string = input_split_list[1]
            # Read File From Local Disk
            fal_status = self.File_Abstract_Layer.fa_read_file(file_string)

            if fal_status:
                # Get File Buffer Bytes
                file_buffer_bytes = self.File_Abstract_Layer.file_buffer_data
                # Get Absolute File Name
                file_name = Path(file_string).name
                # Set Up Upload Network Message
                net_message = (command_fetch, (file_name, file_buffer_bytes))
                # Send To Server And Get Response
                net_response = node_send_network_message(net_message, self.net_host_endpoint)
                # Print Status
                print(f'\n<--- NET-RESPONSE ---> File Upload Status: {net_response}\n')

            return True

        if command_fetch == 'DELETE' and len(input_split_list) > 1:
            # file_string = input_split_list[1]
            # TO BE IMPLEMENTED
            return True

        if command_fetch == 'DOWNLOAD' and len(input_split_list) > 1:
            # Get File String
            file_string = input_split_list[1]
            # Get Absolute File Name
            file_name = Path(file_string).name
            # Set Up Download Network Message
            net_message = (command_fetch, (self.listen_socket.getsockname(), file_name))
            # Send Message To Server, And Skip Response
            node_send_network_message(net_message, self.net_host_endpoint, False, True)
            return True

        if command_fetch == 'COMMANDS' or command_fetch == 'HELP' or command_fetch == 'CMD':
            # Print Rules Or Commands
            print(self.cmd_rules_print)

        # Command Not Supported
        return False

    @staticmethod
    def cl_node_event_handler(client_data):
        """
        cl_node_event_handler() Network Events And Inputs Handling
        :param client_data: Data From Network
        :return: None
        """
        # Get Message Header
        message_header = client_data[0]
        # Get Message Data
        message_data = client_data[1]

        if message_header == 'RETURNDATA':
            # Extrapolate File Name
            server_returned_name = message_data[0]
            # Extrapolate File Bytes
            server_returned_bytes = message_data[1]
            # Print Status
            print(f'\n<--- NET-RESPONSE ---> File: {server_returned_name} Download Received From Host\n')

        if message_header == 'RETURNLS':
            # Extrapolate File Name
            server_returned_name = message_data[0]
            # Extrapolate LS
            server_file_list = message_data[1]
            # Print Status
            print(f'\n<--- NET-RESPONSE ---> List of Saved Files: \n {server_file_list}\n')

        # Command Not Supported
        return None

    def cl_node_commands_main(self):
        """
        cl_node_commands_main() User Command Handler Runner
        :return: None
        """

        while self.continue_run:
            # Get User Input
            user_commands = input(r"--> What Are Your Commands: ").upper()
            # Send To Command Handler
            net_status = self.cl_node_commands_handler(user_commands)
            # Print Status
            print(f'\n--> Your Commands Are: {user_commands},Length: {len(user_commands)},Status: {net_status}\n')

    def cl_node_listen_main(self):
        """
        cl_node_listen_main() Listener Server Runner
        :return: None
        """

        while self.continue_run:
            # Set Timeout
            self.listen_socket.settimeout(2)
            try:
                # Accept Connection
                client_con_socket, client_con_address = self.listen_socket.accept()
            except TimeoutError as error:
                pass
            else:
                # Decode the Message from Client Connection Socket
                client_con_data = node_get_response_sync(client_con_socket, True)

                if client_con_data is not None:
                    # Event Handler Callback
                    self.cl_node_event_handler(client_con_data)

                # close connection
                client_con_socket.close()


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python client.py EXISTINGPORT\nRun main.py first to get a port number")
        exit(1)
    # Random ID
    runner_id = randrange(33, 122)
    # Port Number
    port_num = int(sys.argv[1])
    NodeRunner = ClientNodeProtocol(port_num, runner_id)
