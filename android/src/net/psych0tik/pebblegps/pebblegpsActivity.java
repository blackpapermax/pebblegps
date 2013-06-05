package net.psych0tik.pebblegps;

import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;

import android.view.View;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import android.app.Activity;
import android.os.Bundle;

import java.util.UUID;

/**
 * Tiny activity which updates the GPS coords of the phone on the watch when
 * asked to
 *
 * @author richo@psych0tik.net
 */

public class pebblegpsActivity extends Activity {
    private static final int LAT_KEY = 0;
    private static final int LON_KEY = 1;

    private static final UUID PEBBLEGPS_UUID = UUID.fromString("6398814c-c997-475a-a26a-0db80ae1f1c0");


    private LocationManager mLocationManager;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pebblegps);
        mLocationManager = (LocationManager) this.getSystemService(Context.LOCATION_SERVICE);
    }

    public void updateWeather(View view) {
        LocationListener locationListener = new LocationListener() {
            public void onLocationChanged(Location location) {
                mLocationManager.removeUpdates(this);
                updatePebbleLocation(location);
            }

            public void onStatusChanged(String provider, int status, Bundle extras) {
            }

            public void onProviderEnabled(String provider) {
            }

            public void onProviderDisabled(String provider) {
            }
        };

        mLocationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 0, 0, locationListener);
    }

    public void updatePebbleLocation(Location location) {
        // A very sketchy, rough way of getting the local weather forecast from the phone's approximate location
        // using the OpenWeatherMap webservice: http://openweathermap.org/wiki/API/JSON_API
        double latitude = location.getLatitude();
        double longitude = location.getLongitude();

        PebbleDictionary data = new PebbleDictionary();
        data.addDouble(LAT_KEY, latitude);
        data.addDouble(LON_KEY, longitude);

        PebbleKit.sendDataToPebble(getApplicationContext(), PEBBLEGPS_UUID, data);
    }
}
