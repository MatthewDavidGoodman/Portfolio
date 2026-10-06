"""Short-encounter Gaussian collision probability; covariance is supplied, never inferred from TLEs."""
import numpy as np
from scipy.integrate import quad
from scipy.special import ndtr
from scipy.optimize import brentq

def collision_probability(relative_position_m, relative_velocity_mps, combined_covariance_m2, radius_m):
    r = np.asarray(relative_position_m, dtype=float); v = np.asarray(relative_velocity_mps,dtype=float)
    covariance = np.asarray(combined_covariance_m2,dtype=float)
    if r.shape!=(3,) or v.shape!=(3,) or covariance.shape!=(3,3) or not np.isfinite(np.r_[r,v,covariance.ravel(),radius_m]).all():
        raise ValueError('Finite 3-vectors and 3x3 covariance required')
    if radius_m<=0 or np.linalg.norm(v)<1e-6 or not np.allclose(covariance,covariance.T,atol=1e-12):
        raise ValueError('Positive hard-body radius, encounter speed, symmetric covariance required')
    np.linalg.cholesky(covariance)
    normal=v/np.linalg.norm(v); axis=np.eye(3)[np.argmin(abs(normal))]
    e1=np.cross(normal,axis); e1/=np.linalg.norm(e1); e2=np.cross(normal,e1)
    plane=np.array([e1,e2]); c=plane@covariance@plane.T
    variance,basis=np.linalg.eigh(c); mean=basis.T@plane@r; sigma=np.sqrt(variance)
    def integrand(x):
        half=np.sqrt(max(0,radius_m**2-x*x))
        pdf=np.exp(-0.5*((x-mean[0])/sigma[0])**2)/(np.sqrt(2*np.pi)*sigma[0])
        return pdf*(ndtr((half-mean[1])/sigma[1])-ndtr((-half-mean[1])/sigma[1]))
    probability,error=quad(integrand,-radius_m,radius_m,epsabs=1e-12,epsrel=1e-8,limit=200)
    return {'probability':float(probability),'quadrature_error':float(error),
            'projected_miss_m':float(np.linalg.norm(plane@r)), 'relative_speed_mps':float(np.linalg.norm(v))}

def closest_approach(state_a, state_b, start, stop, samples=1001):
    def derivative(t):
        relative=np.asarray(state_a(t))-state_b(t)
        return float(relative[:3]@relative[3:])
    grid=np.linspace(start,stop,samples); roots=[start,stop]
    for a,b in zip(grid[:-1],grid[1:]):
        if derivative(a)<=0 and derivative(b)>=0:
            roots.append(brentq(derivative,a,b))
    t=min(roots,key=lambda t:np.linalg.norm((np.asarray(state_a(t))-state_b(t))[:3]))
    return t, np.asarray(state_a(t))-state_b(t)

def sgp4_states(line1,line2,julian_dates):
    from sgp4.api import Satrec
    satellite=Satrec.twoline2rv(line1,line2)
    jd=np.asarray(julian_dates,dtype=float); whole=np.floor(jd)
    errors,r,v=satellite.sgp4_array(whole,jd-whole)
    if np.any(errors): raise ValueError(f'SGP4 errors: {np.unique(errors[errors!=0]).tolist()}')
    return {'frame':'TEME','position_m':r*1000,'velocity_mps':v*1000,
            'branch':'deep-space' if satellite.method=='d' else 'near-Earth'}

def teme_to_gcrs(position_m,velocity_mps,julian_dates):
    from astropy.coordinates import TEME,GCRS,CartesianRepresentation,CartesianDifferential
    from astropy.time import Time
    import astropy.units as u
    from astropy.utils import iers
    iers.conf.auto_download=False
    t=Time(julian_dates,format='jd',scale='utc')
    rep=CartesianRepresentation(np.asarray(position_m).T*u.m,
          differentials=CartesianDifferential(np.asarray(velocity_mps).T*u.m/u.s))
    state=TEME(rep,obstime=t).transform_to(GCRS(obstime=t))
    return np.c_[state.cartesian.xyz.to_value(u.m).T,state.velocity.d_xyz.to_value(u.m/u.s).T]
