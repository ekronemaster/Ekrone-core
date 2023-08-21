## Ekrone Infinity Systemctl Daemon

Make sure your Ekrone Infinity infrastructure keeps running after a reboot or after any failure. For this we recommand systemd

Make sure you have the binaries downloaded in advance.

To use the Ekrone Infinity Daemon Service, update the `ekroned.service` file.
Next, copy the `ekroned.service` file to `/etc/systemd/system/ekroned.service`.
After that, copy the `ekroned` binary to `/usr/bin/ekroned`.

Now test the setup with `sudo systemctl start ekroned`.
To check the status, run `sudo systemctl status ekroned`.
Once it works, run `sudo systemctl enable ekroned` to make sure ekroned starts at boot and after a failure.

Restarting: `sudo systemctl restart ekroned`
Stopping: `sudo systemctl stop ekroned`
Starting: `sudo systemctl start ekroned`

*Same process goes for `ekrone-walletd.service`*

### Running as a remote node use

`ExecStart=/usr/bin/ekroned --data-dir=/home/YOUR_USER/.ekrone --rpc-bind-ip=0.0.0.0 --rpc-bind-port=33000`
