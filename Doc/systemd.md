## Ekrone Gold Systemctl Daemon

Make sure your Ekrone Gold infrastructure keeps running after a reboot or after any failure. For this we recommand systemd

Make sure you have the binaries downloaded in advance.

To use the Ekrone Gold Daemon Service, update the `Ekroned.service` file.
Next, copy the `ekroned.service` file to `/etc/systemd/system/Ekroned.service`.
After that, copy the `Ekroned` binary to `/usr/bin/Ekroned`.

Now test the setup with `sudo systemctl start Ekroned`.
To check the status, run `sudo systemctl status Ekroned`.
Once it works, run `sudo systemctl enable Ekroned` to make sure Ekroned starts at boot and after a failure.

Restarting: `sudo systemctl restart Ekroned`
Stopping: `sudo systemctl stop Ekroned`
Starting: `sudo systemctl start Ekroned`

*Same process goes for `Ekrone-walletd.service`*

### Running as a remote node use

`ExecStart=/usr/bin/Ekroned --data-dir=/home/YOUR_USER/.Ekrone --rpc-bind-ip=0.0.0.0 --rpc-bind-port=33000`
