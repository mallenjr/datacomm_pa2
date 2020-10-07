# Client

## Class

### private members

- hostName
- server
- fileName

### public members
<br />

## Main

```
int main(int argc, char *argv[]) {

                        host      port     file
  const Client *client(argv[1], argv[2], argv[3]);

  if (client->initiateTCPHandshake() > 1) {
    cout << "handshake failed." << endl;
    return -1;
  }



  return 0;
}

```