"""
@author Edwin Kaburu
@date 12/3/2022
@see "Seattle University, CPSC 5520, Fall 2022" - Distributed Systems
@file main.py
Lab6 Extra Credit

README:
    * Prototype, Files Are Not Distributed Among the Nodes In Network
        * New Nodes Will Be Accepted And Will Know the Network Topology. But Remain Stale
    * Status Reports Are Displayed.
    * FAL - File Abstraction Layer
        * Responsible For Handling With Files And Their Bytes Of Information
        * Low Level
"""
import sys
from random import randrange
from fileabstraction import *

"""
@class NodeProtocol : Abstraction For Node Accepting Other Node, And Client Wanting to Save Files
"""


class NodeProtocol:
    # Listening Socket
    listen_socket = socket()
    # Existing Known Node Port
    existing_port = 0
    # ID Of Node
    identifier = -1
    # List of Adjacent Nodes. Key -> ID, Value -> Listening-Server-Endpoint
    network_nodes_topology = dict()
    # File Abstraction Layer
    File_Abstract_Layer = FileAbstraction("", True)

    def __init__(self, ext_port, node_id):
        """
        __init__() Default Constructor for class
        :param ext_port: existing port number
        :param node_id: random node ID
        """
        # Initialize Variables
        self.existing_port = ext_port
        self.identifier = node_id
        # Set Up Listening Server
        self.node_proto_init_listen_socket()
        # Join Network
        self.node_proto_join_network()
        # Main Execution On Waiting For Events
        self.node_proto_main()

    def node_proto_update_network_topology(self, ps_net_topology: dict):
        """
        node_proto_update_network_topology() Update Local Network Topology
        :param ps_net_topology: Network Topology Dictionary
        :return: None
        """

        for key, value in ps_net_topology.items():
            # Update Net Topology
            self.network_nodes_topology.update({key: value})

    def node_proto_get_leader_sock_name(self, skip_key=-1):
        """
        node_proto_get_leader_sock_name() Get The Highest Leader From Topology
        :param skip_key: Key To Skip
        :return: Tuple End-Point For Highest Leader In Network
        """
        # Default values
        previous_key = -1
        leader_socket = ('', -1)

        for key, value in self.network_nodes_topology.items():
            # Get The Node with the highest Key
            if key > previous_key and key != skip_key and key != self.identifier:
                # update highest possible leader
                leader_socket = value

        # Return Leader Socket
        return leader_socket

    def node_proto_init_listen_socket(self):
        """
        node_proto_init_listen_socket() Initialize Listener Server
        :return: None
        """
        # Create TCP/IP Listening Socket
        self.listen_socket = socket(AF_INET, SOCK_STREAM)
        # Bind the socket to Port and Name
        self.listen_socket.bind(('localhost', 0))
        # Listen To Connections
        self.listen_socket.listen(5)
        # Debugging Print
        print(f"{self.listen_socket.getsockname()}: Node's Listening Server Set Up Complete")

    def node_proto_join_network(self):
        """
        node_proto_join_network() Attempt To Join Network
        :return: Boolean Condition
        """
        # Add Ourselves To Network Topology Nodes
        self.network_nodes_topology.update({self.identifier: self.listen_socket.getsockname()})

        # Validate Existing Port Value
        if self.existing_port != 0:
            # Print Status
            print(f'{self.listen_socket.getsockname()}: Joining Network Through {self.existing_port}')
            # Exists Other Nodes In Network
            net_message = ('JOIN', (self.identifier, self.network_nodes_topology))
            # Send And Get Response From Existing Node
            net_response = node_send_network_message(net_message, ('localhost', self.existing_port))
            if net_response is not None:
                # Extract Network Topology From Sender
                net_response_data = net_response[1][1]
                # Update Network Topology
                self.node_proto_update_network_topology(net_response_data)
                # Print Status
                print(f'{self.listen_socket.getsockname()}: Accepted To Network')

        return True

    def node_proto_event_handler(self, client_data):
        """
        node_proto_event_handler() Handles Network Events and Inputs
        :param client_data: Network Data
        :return: Object
        """

        # Get Message Header
        message_header = client_data[0]
        # Get Message Data
        message_data = client_data[1]

        # Print Status
        print(f'{self.listen_socket.getsockname()}: Header Message {message_header}')

        if message_header == "JOIN":
            # Extrapolate New Node ID Information
            node_net_id = message_data[0]
            # Extrapolate New Node ID Topology
            node_net_topology = message_data[1]
            # Update Network Topology
            self.node_proto_update_network_topology(node_net_topology)
            # Print Status
            print(f'{self.listen_socket.getsockname()}: Joined Node {node_net_id} Accepted')

        if message_header == "UPDATE":
            # Extrapolate New Node ID Topology
            node_net_topology = message_data[1]
            # Update Network Topology
            self.node_proto_update_network_topology(node_net_topology)
            # Print Status
            print(f'{self.listen_socket.getsockname()}: Update Complete')

            # Debug: Print Current Network Topology
            for key, value in self.network_nodes_topology.items():
                print(f'{key} -> {value}')

            return None

        if message_header == "POLL":
            # Extrapolate New Node ID Topology
            node_net_topology = message_data[1]
            # Update Network Topology
            self.node_proto_update_network_topology(node_net_topology)

            # Inform All To Update
            for key, value in self.network_nodes_topology.items():
                if key != self.identifier:
                    # Print Status
                    print(f'{self.listen_socket.getsockname()}: Update Message Sent To {value}')
                    # Send Update Messages
                    net_message = ('UPDATE', (self.identifier, self.network_nodes_topology))
                    node_send_network_message(net_message, value, False, True)
            return None

        if message_header == 'UPLOAD':
            # Extrapolate File Name
            user_file_name = message_data[0]
            # Extrapolate File Bytes
            user_file_bytes = message_data[1]
            # Save To Local Cache
            fal_status = self.File_Abstract_Layer.fa_write_bytes(user_file_name, user_file_bytes)

            # Return File Status
            if fal_status:
                return f'{self.listen_socket.getsockname()} Saved File'

            return f'{self.listen_socket.getsockname()} Not Saved File'

        if message_header == 'DOWNLOAD':
            # Extrapolate Client Listen EndPoint
            client_skt_name = message_data[0]
            # Extrapolate File Name
            user_file_name = message_data[1]
            # Attempt To Get Bytes
            fal_status = self.File_Abstract_Layer.fa_read_bytes(user_file_name)

            # Send File Data Response To Client
            net_message = 'RETURNDATA', (user_file_name, fal_status)
            node_send_network_message(net_message, client_skt_name, False, True)

        if message_header == 'LS':
            # Get Client Socket
            client_skt_name = message_data[0]
            # Get Values of Dictionary Mappers
            saved_files = ''
            for key, value in self.File_Abstract_Layer.node_net_cached_files_dict.items():
                saved_files += value

            if saved_files == '':
                saved_files = 'Empty List'

            # Return Message Back To Client
            net_message = 'RETURNLS', ('List', saved_files)
            node_send_network_message(net_message, client_skt_name, False, True)

        # Return OK Message
        return 'OK', (self.identifier, self.network_nodes_topology)

    def node_proto_main(self):
        """
        node_proto_main() Main Listener Execution Method
        :return: None
        """

        # Get Leader
        leader_socket = self.node_proto_get_leader_sock_name()

        if leader_socket[0] != '':
            # Print Status
            print(f'{self.listen_socket.getsockname()}: Polling Leader')
            # Polling Message
            net_message = ('POLL', (self.identifier, self.network_nodes_topology))
            # Send Message To Leader
            node_send_network_message(net_message, leader_socket, False, True)

        # Print Status
        print(f'{self.listen_socket.getsockname()}: Ready And Listening For Events')

        while True:
            # Accept Connection
            client_con_socket, client_con_address = self.listen_socket.accept()
            # Decode the Message from Client Connection Socket
            client_con_data = node_get_response_sync(client_con_socket, True)

            if client_con_data is not None:
                # Event Handler Callback
                response_message = self.node_proto_event_handler(client_con_data)

                if response_message is not None:
                    # Send Response Back To Client
                    client_con_socket.send(pickle.dumps(response_message))

            # close connection
            client_con_socket.close()


if __name__ == '__main__':

    if len(sys.argv) != 2:
        print("Usage: python main.py EXISTINGPORT\nIf First Node in Network then:\n"
              "python main.py 0")
        exit(1)

    # Random ID
    runner_id = randrange(2, 22)
    # Port Number
    port_num = int(sys.argv[1])
    NodeRunner = NodeProtocol(port_num, runner_id)
